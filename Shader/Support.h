#pragma once

#include"error.h"
#include"release.h"
#include"Model.h"

class Support
{
private:
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	LPVOID computeblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	size_t computesize_;
	ComPtr<ID3D10Blob> pixelshaderbuffer_;
	ComPtr<ID3D10Blob> vertexshaderbuffer_;
	ComPtr<ID3D10Blob> computeshaderbuffer_;
	std::vector<char> vertexdataarray_;
	std::vector<char> pixeldataarray_;
	std::vector<char> computedataarray_;
	static wchar_t filename_[MAX_PATH];

public:
	Support();
	~Support();

	//Check
	static bool checkInputLayoutData(const void* shadercode, size_t codesize,const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //���_���̓��C�A�E�g���쐬����f�[�^���L�����m�F����֐�
	
	//Search
	//�t�@�C���p�X���L��������i�L���Ȃ�true��Ԃ�)
	static bool searchFile(const wchar_t* FileName);
	static bool searchFile(const char* FileName);
	static bool searchFile(const std::string FileName);

	//Rename
	static wchar_t* renameExtension(const wchar_t* SorcePath,const char* RenameExtension); //�n���ꂽ�t�@�C���p�X�̊g���q��ϊ�����֐� ���g���q��[.jpg]�̂悤�Ƀh�b�g��t����
	
	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName,ID3D11VertexShader** VertexShader);		 			                          //�n���ꂽ�t�@�C�����Œ��_�V�F�[�_�[���쐬
	HRESULT createPixelData(const wchar_t* PixelShaderFileName,ID3D11PixelShader** PixelShader);								                  //�n���ꂽ�t�@�C�����Ńs�N�Z���V�F�[�_�[���쐬
	HRESULT createComputeData(const wchar_t* ComputeShdaerFileName,ID3D11ComputeShader** ComputeShader);										  //�n���ꂽ�t�@�C�����ŃR���s���[�g�V�F�[�_�[���쐬
	HRESULT createVertexInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& PolygonLayoutArray, ID3D11InputLayout** InputLayout);
	//Get
	inline const LPVOID getVertexBufferPtr()const { return vertexblob_; }			//���_�V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const LPVOID getPixelBufferPtr()const { return pixelblob_; }				//�s�N�Z���V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const LPVOID getComputeBufferPtr()const { return computeblob_; }			//�R���s���[�g�V�F�[�_�[�̃o�b�t�@�|�C���^��Ԃ�
	inline const size_t getVertexBufferSize()const { return vertexsize_; }			//���_�V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline const size_t getPixelBufferSize()const { return pixelsize_; }			//�s�N�Z���V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
	inline const size_t getComputeBufferSize()const { return computesize_; }		//�R���s���[�g�V�F�[�_�[�̃o�b�t�@�T�C�Y��Ԃ�
};

