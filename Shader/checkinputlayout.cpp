#include "stdafx.h"
#include "checkinputlayout.h"

bool checkInputLayout(const void* shadercode, size_t codesize, const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum)
{
    ID3D11ShaderReflection* vsref;
    HRESULT hr = D3DReflect(shadercode, codesize, IID_ID3D11ShaderReflection, (void**)&vsref);
    if (FAILED(hr))
    {
        return false;
    }

    D3D11_SHADER_DESC shaderdesc;
    hr = vsref->GetDesc(&shaderdesc);
    if (FAILED(hr))
    {
        vsref->Release();
        return false;
    }

    for (UINT i = 0; i < shaderdesc.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC desc;
        hr = vsref->GetInputParameterDesc(i, &desc);
        if (FAILED(hr))
        {
            vsref->Release();
            return false;
        }

        UINT j;

        for (j = 0; j < layoutnum; ++j)
        {
            if (strcmp(layout[j].SemanticName, desc.SemanticName) == 0)
            {
                break;
            }
        }
        if (j == layoutnum)
        {
            vsref->Release();
            return false;
        }

    }

    vsref->Release();
    return true;
}
