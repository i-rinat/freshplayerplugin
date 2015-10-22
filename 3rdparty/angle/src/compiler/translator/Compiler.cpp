//
// Copyright (c) 2002-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/translator/Cache.h"
#include "compiler/translator/Compiler.h"
#include "compiler/translator/CallDAG.h"
#include "compiler/translator/Initialize.h"
#include "compiler/translator/InitializeParseContext.h"
#include "compiler/translator/ParseContext.h"
#include "compiler/translator/PruneEmptyDeclarations.h"
#include "compiler/translator/ValidateOutputs.h"
#include "angle_gl.h"
#include "common/utilities.h"


size_t GetGlobalMaxTokenSize(ShShaderSpec spec)
{
    return 1024;
}

namespace {

class TScopedPoolAllocator
{
  public:
    TScopedPoolAllocator(TPoolAllocator* allocator) : mAllocator(allocator)
    {
        mAllocator->push();
        SetGlobalPoolAllocator(mAllocator);
    }
    ~TScopedPoolAllocator()
    {
        SetGlobalPoolAllocator(NULL);
        mAllocator->pop();
    }

  private:
    TPoolAllocator* mAllocator;
};

class TScopedSymbolTableLevel
{
  public:
    TScopedSymbolTableLevel(TSymbolTable* table) : mTable(table)
    {
        ASSERT(mTable->atBuiltInLevel());
        mTable->push();
    }
    ~TScopedSymbolTableLevel()
    {
        while (!mTable->atBuiltInLevel())
            mTable->pop();
    }

  private:
    TSymbolTable* mTable;
};

int MapSpecToShaderVersion(ShShaderSpec spec)
{
    return 100;
}

}  // namespace

TShHandleBase::TShHandleBase()
{
    allocator.push();
    SetGlobalPoolAllocator(&allocator);
}

TShHandleBase::~TShHandleBase()
{
    SetGlobalPoolAllocator(NULL);
    allocator.popAll();
}

TCompiler::TCompiler(sh::GLenum type, ShShaderSpec spec, ShShaderOutput output)
    : shaderType(type),
      shaderSpec(spec),
      outputType(output),
      maxUniformVectors(0),
      maxExpressionComplexity(0),
      maxCallStackDepth(0),
      fragmentPrecisionHigh(false),
      builtInFunctionEmulator(),
      mSourcePath(NULL)
{
}

TCompiler::~TCompiler()
{
}

bool TCompiler::Init(const ShBuiltInResources& resources)
{
    shaderVersion = 100;
    maxUniformVectors = (shaderType == GL_VERTEX_SHADER) ?
        resources.MaxVertexUniformVectors :
        resources.MaxFragmentUniformVectors;
    maxExpressionComplexity = resources.MaxExpressionComplexity;
    maxCallStackDepth = resources.MaxCallStackDepth;

    SetGlobalPoolAllocator(&allocator);

    // Generate built-in symbol table.
    if (!InitBuiltInSymbolTable(resources))
        return false;
    InitExtensionBehavior(resources, extensionBehavior);
    fragmentPrecisionHigh = resources.FragmentPrecisionHigh == 1;

    hashFunction = resources.HashFunction;

    return true;
}

TIntermNode *TCompiler::compileTreeForTesting(const char* const shaderStrings[],
    size_t numStrings, int compileOptions)
{
    return compileTreeImpl(shaderStrings, numStrings, compileOptions);
}

TIntermNode *TCompiler::compileTreeImpl(const char* const shaderStrings[],
    size_t numStrings, int compileOptions)
{
    clearResults();

    ASSERT(numStrings > 0);
    ASSERT(GetGlobalPoolAllocator());

    // Reset the extension behavior for each compilation unit.
    ResetExtensionBehavior(extensionBehavior);

    // First string is path of source file if flag is set. The actual source follows.
    size_t firstSource = 0;

    bool debugShaderPrecision = getResources().WEBGL_debug_shader_precision == 1;
    TIntermediate intermediate(infoSink);
    TParseContext parseContext(symbolTable, extensionBehavior, intermediate,
                               shaderType, shaderSpec, compileOptions, true,
                               infoSink, debugShaderPrecision);

    parseContext.setFragmentPrecisionHigh(fragmentPrecisionHigh);
    SetGlobalParseContext(&parseContext);

    // We preserve symbols at the built-in level from compile-to-compile.
    // Start pushing the user-defined symbols at global level.
    TScopedSymbolTableLevel scopedSymbolLevel(&symbolTable);

    // Parse shader.
    bool success =
        (PaParseStrings(numStrings - firstSource, &shaderStrings[firstSource], nullptr, &parseContext) == 0) &&
        (parseContext.getTreeRoot() != nullptr);

    shaderVersion = parseContext.getShaderVersion();
    if (success && MapSpecToShaderVersion(shaderSpec) < shaderVersion)
    {
        infoSink.info.prefix(EPrefixError);
        infoSink.info << "unsupported shader version";
        success = false;
    }

    TIntermNode *root = nullptr;

    if (success)
    {
        mPragma = parseContext.pragma();
        if (mPragma.stdgl.invariantAll)
        {
            symbolTable.setGlobalInvariant();
        }

        root = parseContext.getTreeRoot();
        root = intermediate.postProcess(root);

        // Create the function DAG and check there is no recursion
        if (success)
            success = initCallDag(root);

        // Checks which functions are used and if "main" exists
        if (success)
        {
            functionMetadata.clear();
            functionMetadata.resize(mCallDag.size());
            success = tagUsedFunctions();
        }

        // Prune empty declarations to work around driver bugs and to keep declaration output simple.
        if (success)
            PruneEmptyDeclarations(root);

        if (success && shaderVersion == 300 && shaderType == GL_FRAGMENT_SHADER)
            success = validateOutputs(root);

        // Built-in function emulation needs to happen after validateLimitations pass.
        if (success)
        {
            initBuiltInFunctionEmulator(&builtInFunctionEmulator, compileOptions);
            builtInFunctionEmulator.MarkBuiltInFunctionsForEmulation(root);
        }
    }

    SetGlobalParseContext(NULL);
    if (success)
        return root;

    return NULL;
}

bool TCompiler::compile(const char* const shaderStrings[],
    size_t numStrings, int compileOptions)
{
    if (numStrings == 0)
        return true;

    TScopedPoolAllocator scopedAlloc(&allocator);
    TIntermNode *root = compileTreeImpl(shaderStrings, numStrings, compileOptions);

    if (root)
    {
        if (compileOptions & SH_OBJECT_CODE)
            translate(root, compileOptions);

        // The IntermNode tree doesn't need to be deleted here, since the
        // memory will be freed in a big chunk by the PoolAllocator.
        return true;
    }
    return false;
}

bool TCompiler::InitBuiltInSymbolTable(const ShBuiltInResources &resources)
{
    compileResources = resources;
    setResourceString();

    assert(symbolTable.isEmpty());
    symbolTable.push();   // COMMON_BUILTINS
    symbolTable.push();   // ESSL1_BUILTINS
    symbolTable.push();   // ESSL3_BUILTINS

    TPublicType integer;
    integer.type = EbtInt;
    integer.primarySize = 1;
    integer.secondarySize = 1;
    integer.array = false;

    TPublicType floatingPoint;
    floatingPoint.type = EbtFloat;
    floatingPoint.primarySize = 1;
    floatingPoint.secondarySize = 1;
    floatingPoint.array = false;

    TPublicType sampler;
    sampler.primarySize = 1;
    sampler.secondarySize = 1;
    sampler.array = false;

    switch(shaderType)
    {
      case GL_FRAGMENT_SHADER:
        symbolTable.setDefaultPrecision(integer, EbpMedium);
        break;
      case GL_VERTEX_SHADER:
        symbolTable.setDefaultPrecision(integer, EbpHigh);
        symbolTable.setDefaultPrecision(floatingPoint, EbpHigh);
        break;
      default:
        assert(false && "Language not supported");
    }
    // We set defaults for all the sampler types, even those that are
    // only available if an extension exists.
    for (int samplerType = EbtGuardSamplerBegin + 1;
         samplerType < EbtGuardSamplerEnd; ++samplerType)
    {
        sampler.type = static_cast<TBasicType>(samplerType);
        symbolTable.setDefaultPrecision(sampler, EbpLow);
    }

    InsertBuiltInFunctions(shaderType, shaderSpec, resources, symbolTable);

    IdentifyBuiltIns(shaderType, shaderSpec, resources, symbolTable);

    return true;
}

void TCompiler::setResourceString()
{
    std::ostringstream strstream;
    strstream << ":MaxVertexAttribs:" << compileResources.MaxVertexAttribs
              << ":MaxVertexUniformVectors:" << compileResources.MaxVertexUniformVectors
              << ":MaxVaryingVectors:" << compileResources.MaxVaryingVectors
              << ":MaxVertexTextureImageUnits:" << compileResources.MaxVertexTextureImageUnits
              << ":MaxCombinedTextureImageUnits:" << compileResources.MaxCombinedTextureImageUnits
              << ":MaxTextureImageUnits:" << compileResources.MaxTextureImageUnits
              << ":MaxFragmentUniformVectors:" << compileResources.MaxFragmentUniformVectors
              << ":MaxDrawBuffers:" << compileResources.MaxDrawBuffers
              << ":OES_standard_derivatives:" << compileResources.OES_standard_derivatives
              << ":OES_EGL_image_external:" << compileResources.OES_EGL_image_external
              << ":ARB_texture_rectangle:" << compileResources.ARB_texture_rectangle
              << ":EXT_draw_buffers:" << compileResources.EXT_draw_buffers
              << ":FragmentPrecisionHigh:" << compileResources.FragmentPrecisionHigh
              << ":MaxExpressionComplexity:" << compileResources.MaxExpressionComplexity
              << ":MaxCallStackDepth:" << compileResources.MaxCallStackDepth
              << ":EXT_blend_func_extended:" << compileResources.EXT_blend_func_extended
              << ":EXT_frag_depth:" << compileResources.EXT_frag_depth
              << ":EXT_shader_texture_lod:" << compileResources.EXT_shader_texture_lod
              << ":EXT_shader_framebuffer_fetch:" << compileResources.EXT_shader_framebuffer_fetch
              << ":NV_shader_framebuffer_fetch:" << compileResources.NV_shader_framebuffer_fetch
              << ":ARM_shader_framebuffer_fetch:" << compileResources.ARM_shader_framebuffer_fetch
              << ":MaxVertexOutputVectors:" << compileResources.MaxVertexOutputVectors
              << ":MaxFragmentInputVectors:" << compileResources.MaxFragmentInputVectors
              << ":MinProgramTexelOffset:" << compileResources.MinProgramTexelOffset
              << ":MaxProgramTexelOffset:" << compileResources.MaxProgramTexelOffset
              << ":MaxDualSourceDrawBuffers:" << compileResources.MaxDualSourceDrawBuffers
              << ":NV_draw_buffers:" << compileResources.NV_draw_buffers
              << ":WEBGL_debug_shader_precision:" << compileResources.WEBGL_debug_shader_precision;

    builtInResourcesString = strstream.str();
}

void TCompiler::clearResults()
{
    infoSink.info.erase();
    infoSink.obj.erase();
    infoSink.debug.erase();

    builtInFunctionEmulator.Cleanup();

    nameMap.clear();

    mSourcePath = NULL;
}

bool TCompiler::initCallDag(TIntermNode *root)
{
    mCallDag.clear();

    switch (mCallDag.init(root, &infoSink.info))
    {
      case CallDAG::INITDAG_SUCCESS:
        return true;
      case CallDAG::INITDAG_RECURSION:
        infoSink.info.prefix(EPrefixError);
        infoSink.info << "Function recursion detected";
        return false;
      case CallDAG::INITDAG_UNDEFINED:
        infoSink.info.prefix(EPrefixError);
        infoSink.info << "Unimplemented function detected";
        return false;
    }

    UNREACHABLE();
    return true;
}

bool TCompiler::tagUsedFunctions()
{
    // Search from main, starting from the end of the DAG as it usually is the root.
    for (size_t i = mCallDag.size(); i-- > 0;)
    {
        if (mCallDag.getRecordFromIndex(i).name == "main(")
        {
            internalTagUsedFunction(i);
            return true;
        }
    }

    infoSink.info.prefix(EPrefixError);
    infoSink.info << "Missing main()";
    return false;
}

void TCompiler::internalTagUsedFunction(size_t index)
{
    if (functionMetadata[index].used)
    {
        return;
    }

    functionMetadata[index].used = true;

    for (int calleeIndex : mCallDag.getRecordFromIndex(index).callees)
    {
        internalTagUsedFunction(calleeIndex);
    }
}

// A predicate for the stl that returns if a top-level node is unused
class TCompiler::UnusedPredicate
{
  public:
    UnusedPredicate(const CallDAG *callDag, const std::vector<FunctionMetadata> *metadatas)
        : mCallDag(callDag),
          mMetadatas(metadatas)
    {
    }

    bool operator ()(TIntermNode *node)
    {
        const TIntermAggregate *asAggregate = node->getAsAggregate();

        if (asAggregate == nullptr)
        {
            return false;
        }

        if (!(asAggregate->getOp() == EOpFunction || asAggregate->getOp() == EOpPrototype))
        {
            return false;
        }

        size_t callDagIndex = mCallDag->findIndex(asAggregate);
        if (callDagIndex == CallDAG::InvalidIndex)
        {
            // This happens only for unimplemented prototypes which are thus unused
            ASSERT(asAggregate->getOp() == EOpPrototype);
            return true;
        }

        ASSERT(callDagIndex < mMetadatas->size());
        return !(*mMetadatas)[callDagIndex].used;
    }

  private:
    const CallDAG *mCallDag;
    const std::vector<FunctionMetadata> *mMetadatas;
};

bool TCompiler::validateOutputs(TIntermNode* root)
{
    ValidateOutputs validateOutputs(getExtensionBehavior(), compileResources.MaxDrawBuffers);
    root->traverse(&validateOutputs);
    return (validateOutputs.validateAndCountErrors(infoSink.info) == 0);
}

const TExtensionBehavior& TCompiler::getExtensionBehavior() const
{
    return extensionBehavior;
}

const char *TCompiler::getSourcePath() const
{
    return mSourcePath;
}

const ShBuiltInResources& TCompiler::getResources() const
{
    return compileResources;
}

const BuiltInFunctionEmulator& TCompiler::getBuiltInFunctionEmulator() const
{
    return builtInFunctionEmulator;
}

void TCompiler::writePragma()
{
    TInfoSinkBase &sink = infoSink.obj;
    if (mPragma.stdgl.invariantAll)
        sink << "#pragma STDGL invariant(all)\n";
}
