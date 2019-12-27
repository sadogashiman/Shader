#pragma once
#include"Shadowshader.h"
#include"Alphamapshader.h"
#include"Bumpmap.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Deferredbuffers.h"
#include"Light.h"

class Renderer
{
private:
	Renderer() = default;
	~Renderer() = default;
	std::unique_ptr<LightShader> lightshader_;
	std::unique_ptr<Bumpmap> bumpshader_;
	std::unique_ptr<Alphamapshader> alphashader_;
	std::unique_ptr<Shadowshader> shadowshader_;
	std::unique_ptr<Depthshader> depthshader_;
	std::unique_ptr<Deferredbuffers>deferredbuffer_;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;
public:


	bool init();
	void destroy();

	static inline Renderer* getInstance()
	{
		static Renderer instance;
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
	bool shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture);

	//�e�N�X�`���V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�A�e�s��A�e�N�X�`��)
	bool textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//�}���`�e�N�X�`�������O���g�p����Ƃ��̃����_�[(���f���̃C���f�b�N�X�J�E���g�A�e�s��A�e�N�X�`�����񖇓������z��)
	bool multiTexRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);
};

