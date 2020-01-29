#pragma once

#include"error.h"
#include"release.h"


class Support
{
private:
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	ID3D10Blob* pixelshaderbuffer_;
	ID3D10Blob* vertexshaderbuffer_;
	std::vector<char> vertexdataarray_;
	std::vector<char> pixeldataarray_;
	static wchar_t filename_[MAX_PATH];

public:
	Support();
	~Support();

	//Check
	static bool checkInputLayout(const void* shadercode, size_t codesize,
								 const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //���_���̓��C�A�E�g���쐬����f�[�^���L�����m�F����֐�
	//Search
	static bool searchFile(const wchar_t* FileName);
	static bool searchFile(const char* FileName);
	static bool searchFile(const std::string FileName);

	//Rename
	static wchar_t* renameToImageFileName(const wchar_t* ModelFileName);

	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName);					//�n���ꂽ�t�@�C�����Œ��_�V�F�[�_�[���쐬
	HRESULT createPixelData(const wchar_t* PixelShaderFileName);					//�n���ꂽ�t�@�C�����Ńs�N�Z���V�F�[�_�[���쐬

	//Get
	inline const LPVOID getVertexBufferPtr()const { return vertexblob_; }			//���_�V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const LPVOID getPixelBufferPtr()const { return pixelblob_; }			//�s�N�Z���V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const size_t getVertexBufferSize()const { return vertexsize_; }			//���_�V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline const size_t getPixelBufferSize()const { return pixelsize_; }			//�s�N�Z���V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline ID3D11VertexShader* getVertexShader()const { return vertexshader_; }		//�쐬���ꂽ���_�V�F�[�_�[�̃|�C���^�[��Ԃ�
	inline ID3D11PixelShader* getPixelShader()const { return pixelshader_; }		//�쐬�����s�N�Z���V�F�[�_�[�̃|�C���^�[��Ԃ�

	//destroy
	void destroyBufferData();	//�s�v�ɂȂ����o�b�t�@�f�[�^���폜
};

