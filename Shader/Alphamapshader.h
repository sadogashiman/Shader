#pragma once
class Alphamapshader
{
private:
    struct MatrixBufferType
    {
        Matrix world;
        Matrix view;
        Matrix projection;
    };
    bool initshader( const wchar_t* vsFileName, const wchar_t* psFileName);
    void destroyshader();
    bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);
    void rendershader(int Indexcount);
    ID3D11VertexShader* vertexshader_;
    ID3D11PixelShader* pixelshader_;
    ID3D11InputLayout* layout_;
    ID3D11Buffer* matrixbuffer_;
    ID3D11SamplerState* samplestate_;
public:
    Alphamapshader();
    ~Alphamapshader();
    bool init();
    void destroy();
    bool render(int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView**TextureArray);
};

