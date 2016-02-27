include_directories(include)
include_directories(src)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")

add_library(shader_translator STATIC
    src/common/Float16ToFloat32.cpp
    src/common/tls.cpp
    src/common/utilities.cpp
    src/compiler/preprocessor/DiagnosticsBase.cpp
    src/compiler/preprocessor/DirectiveHandlerBase.cpp
    src/compiler/preprocessor/DirectiveParser.cpp
    src/compiler/preprocessor/ExpressionParser.cpp
    src/compiler/preprocessor/Input.cpp
    src/compiler/preprocessor/Lexer.cpp
    src/compiler/preprocessor/Macro.cpp
    src/compiler/preprocessor/MacroExpander.cpp
    src/compiler/preprocessor/Preprocessor.cpp
    src/compiler/preprocessor/Token.cpp
    src/compiler/preprocessor/Tokenizer.cpp
    src/compiler/translator/BuiltInFunctionEmulator.cpp
    src/compiler/translator/BuiltInFunctionEmulatorGLSL.cpp
    src/compiler/translator/Cache.cpp
    src/compiler/translator/CallDAG.cpp
    src/compiler/translator/CodeGen.cpp
    src/compiler/translator/Compiler.cpp
    src/compiler/translator/Diagnostics.cpp
    src/compiler/translator/DirectiveHandler.cpp
    src/compiler/translator/EmulatePrecision.cpp
    src/compiler/translator/glslang_lex.cpp
    src/compiler/translator/glslang_tab.cpp
    src/compiler/translator/InfoSink.cpp
    src/compiler/translator/Initialize.cpp
    src/compiler/translator/InitializeDll.cpp
    src/compiler/translator/InitializeParseContext.cpp
    src/compiler/translator/Intermediate.cpp
    src/compiler/translator/intermOut.cpp
    src/compiler/translator/IntermNode.cpp
    src/compiler/translator/IntermTraverse.cpp
    src/compiler/translator/LoopInfo.cpp
    src/compiler/translator/Operator.cpp
    src/compiler/translator/OutputGLSLBase.cpp
    src/compiler/translator/OutputGLSL.cpp
    src/compiler/translator/parseConst.cpp
    src/compiler/translator/ParseContext.cpp
    src/compiler/translator/PoolAlloc.cpp
    src/compiler/translator/PruneEmptyDeclarations.cpp
    src/compiler/translator/RecordConstantPrecision.cpp
    src/compiler/translator/ShaderLang.cpp
    src/compiler/translator/SymbolTable.cpp
    src/compiler/translator/TranslatorGLSL.cpp
    src/compiler/translator/Types.cpp
    src/compiler/translator/util.cpp
    src/compiler/translator/ValidateGlobalInitializer.cpp
    src/compiler/translator/ValidateOutputs.cpp
    src/compiler/translator/ValidateSwitch.cpp
    src/compiler/translator/VersionGLSL.cpp
)

add_executable(st samples/shader_translator/shader_translator.cpp)
target_link_libraries(st shader_translator)
