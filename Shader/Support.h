#pragma once

#include"error.h"
#include"release.h"


class Support
{
private:
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11ComputeShader* computeshader_;
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	LPVOID computeblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	size_t computesize_;
	ComPtr<ID3D10Blob> pixelshaderbuffer_;
	ComPtr<ID3D10Blob> vertexshaderbuffer_;
	ComPtr<ID3D10Blob> computeshaderbuffer_;
	ID3D11InputLayout* layout_;
	std::vector<char> vertexdataarray_;
	std::vector<char> pixeldataarray_;
	std::vector<char> computedataarray_;
	static wchar_t filename_[MAX_PATH];
	static wchar_t mtlfilename[MAX_PATH];

public:
	Support();
	~Support();

	//Check
	static bool checkInputLayoutData(const void* shadercode, size_t codesize,
								 const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //���_���̓��C�A�E�g���쐬����f�[�^���L�����m�F����֐�
	//Search
	//�t�@�C���p�X���L��������i�L���Ȃ�true��Ԃ�)
	static bool searchFile(const wchar_t* FileName);
	static bool searchFile(const char* FileName);
	static bool searchFile(const std::string FileName);

	//Rename
	static wchar_t* renameToImageFileName(const wchar_t* ModelFileName);			//�n���ꂽ�t�@�C�����̊g���q���f�B���N�g�����ɑ��݂��铯���̉摜�t�@�C���g���q�ɕύX����
	static wchar_t* renameToMaterialFileName(const wchar_t* MaterialFileName);		//�n���ꂽ�t�@�C���p�X��������g���q.mtl�ɕϊ�����
	
	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName);		 			//�n���ꂽ�t�@�C�����Œ��_�V�F�[�_�[���쐬
	HRESULT createPixelData(const wchar_t* PixelShaderFileName);					//�n���ꂽ�t�@�C�����Ńs�N�Z���V�F�[�_�[���쐬
	HRESULT createComputeData(const wchar_t* ComputeShdaerFileName);
	HRESULT createVertexInputLayout(D3D11_INPUT_ELEMENT_DESC* PolygonLayoutArray,const unsigned int NumElements); //�n���ꂽ���Œ��_���̓��C�A�E�g���쐬(�f�o�b�N���̓f�[�^�`�F�b�N���s��)

	//Get
	inline const LPVOID getVertexBufferPtr()const { return vertexblob_; }			//���_�V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const LPVOID getPixelBufferPtr()const { return pixelblob_; }				//�s�N�Z���V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const LPVOID getcomputeBufferPtr()const { return computeblob_; }			//�R���s���[�g�V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const size_t getVertexBufferSize()const { return vertexsize_; }			//���_�V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline const size_t getPixelBufferSize()const { return pixelsize_; }			//�s�N�Z���V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline const size_t getComputeBufferSize()const { return computesize_; }		//�R���s���[�g�V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline ID3D11VertexShader* getVertexShader()const { return vertexshader_; }		//�쐬���ꂽ���_�V�F�[�_�[�̃|�C���^�[��Ԃ�
	inline ID3D11PixelShader* getPixelShader()const { return pixelshader_; }		//�쐬�����s�N�Z���V�F�[�_�[�̃|�C���^�[��Ԃ�
	inline ID3D11ComputeShader* getComputeShader()const { return computeshader_; }  //�쐬�����R���s���[�g�V�F�[�_�[�̃|�C���^��Ԃ�
	inline ID3D11InputLayout* getInputLayout()const { return layout_; }				//�쐬�������_���̓��C�A�E�g�̃|�C���^��Ԃ�
};

