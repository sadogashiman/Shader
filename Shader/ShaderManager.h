#pragma once
#include"Shadowshader.h"
#include"Maskshader.h"
#include"Bumpmap.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Light.h"
#include"TextureShader.h"
#include"Deferredshader.h"
#include"Multitexture.h"

class ShaderManager
{
private:
	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;
	ShaderManager(ShaderManager&&) = delete;
	ShaderManager& operator=(ShaderManager&&) = delete;

	std::unique_ptr<LightShader> lightshader_;
	std::unique_ptr<Bumpmap> bumpshader_;
	std::unique_ptr<Maskshader> maskshader_;
	std::unique_ptr<Shadowshader> shadowshader_;
	std::unique_ptr<Depthshader> depthshader_;
	std::unique_ptr<Textureshader>textureshader_;
	std::unique_ptr<Deferredshader>deferredshader_;
	std::unique_ptr<Multitexture>multitexshader_;

public:

	bool init();
	void destroy();

	static inline ShaderManager* getInstance()
	{
		static ShaderManager instance;
		return &instance;
	}

	//�}�X�N���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�e�s��A�e�N�X�`���񖇂ƃA���t�@�}�b�v�̓������e�N�X�`���z��)
	bool maskRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);

	//�o���v�}�b�v���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�e�s��A�e�N�X�`���񖇂Ɩ@���}�b�v�̓������e�N�X�`���z��A���C�g�̃p�����[�^�[)
	bool bumpRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata);

	//�f�v�X�}�b�v���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�e�s��)
	bool depthRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection);

	//���C�g�V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�A�e�s��A�e�N�X�`���A���C�g�̃p�����[�^�[)
	bool lightRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* Lightdata);

	//�V���h�E�V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�A�e�s��A�e�N�X�`���A�f�v�X�}�b�v�̃e�N�X�`��)
	bool shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);

	//�e�N�X�`���V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�A�e�s��A�e�N�X�`��)
	bool textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//�x���V�F�[�f�B���O���g�p����Ƃ��̃����_�[
	bool deferredRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//�}���`�e�N�X�`�������O���g�p����Ƃ��̃����_�[
	bool multiTextureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum);
};

