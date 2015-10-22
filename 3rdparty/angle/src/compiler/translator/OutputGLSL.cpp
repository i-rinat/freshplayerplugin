//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/translator/OutputGLSL.h"

TOutputGLSL::TOutputGLSL(TInfoSinkBase& objSink,
                         ShHashFunction64 hashFunction,
                         NameMap& nameMap,
                         TSymbolTable& symbolTable,
                         int shaderVersion,
                         ShShaderOutput output)
    : TOutputGLSLBase(objSink,
                      hashFunction,
                      nameMap,
                      symbolTable,
                      shaderVersion,
                      output)
{
}

bool TOutputGLSL::writeVariablePrecision(TPrecision)
{
    return false;
}

void TOutputGLSL::visitSymbol(TIntermSymbol *node)
{
    TInfoSinkBase& out = objSink();

    const TString &symbol = node->getSymbol();
    if (symbol == "gl_FragDepthEXT")
    {
        out << "gl_FragDepth";
    }
    else if (symbol == "gl_SecondaryFragColorEXT")
    {
        out << "angle_SecondaryFragColor";
    }
    else if (symbol == "gl_SecondaryFragDataEXT")
    {
        out << "angle_SecondaryFragData";
    }
    else
    {
        TOutputGLSLBase::visitSymbol(node);
    }
}

TString TOutputGLSL::translateTextureFunction(TString &name)
{
    static const char *simpleRename[] = {
        "texture2DLodEXT", "texture2DLod",
        "texture2DProjLodEXT", "texture2DProjLod",
        "textureCubeLodEXT", "textureCubeLod",
        "texture2DGradEXT", "texture2DGradARB",
        "texture2DProjGradEXT", "texture2DProjGradARB",
        "textureCubeGradEXT", "textureCubeGradARB",
        NULL, NULL
    };
    const char **mapping = simpleRename;

    for (int i = 0; mapping[i] != NULL; i += 2)
    {
        if (name == mapping[i])
        {
            return mapping[i+1];
        }
    }

    return name;
}
