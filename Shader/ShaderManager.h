#pragma once

//�V�F�[�_�[
#include"Shadowshader.h"
#include"Maskshader.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Light.h"
#include"TextureShader.h"
#include"Deferredshader.h"
#include"MultitexShader.h"
#include"Skydomeshader.h"
#include"Colorshader.h"
#include"SkyplaneShader.h"
#include"TerrainShader.h"
#include"ParticleShader.h"
#include"SpotlightShader.h"

//���f��
#include"Model3D.h"
#include"Model.h"
#include"SkyDome.h"
#include"Skyplane.h"
#include"Terrain.h"
#include"ParticleSystem.h"


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
	std::unique_ptr<Maskshader> maskshader_;
	std::unique_ptr<Shadowshader> shadowshader_;
	std::unique_ptr<Depthshader> depthshader_;
	std::unique_ptr<Textureshader> textureshader_;
	std::unique_ptr<Deferredshader> deferredshader_;
	std::unique_ptr<MultitexShader> multitexshader_;
	std::unique_ptr<Skydomeshader> skydomeshader_;
	std::unique_ptr<Colorshader> colorshader_;
	std::unique_ptr<SkyplaneShader> skyplaneshader_;
	std::unique_ptr<TerrainShader> terrainshader_;
	std::unique_ptr<ParticleShader> particleshader_;
	std::unique_ptr<SpotlightShader> spotlightshader_;
public:

	bool init();

	static inline ShaderManager* getInstance()
	{
		static ShaderManager instance;
		return &instance;
	}

	//�J���[�V�F�[�_�[���g�p����Ƃ��̃����_�[
	bool colorRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection,Vector4 Color);

	//�}�X�N���g�p����Ƃ��̃����_�[(���f���̃|�C���^�A�e�s��A�e�N�X�`���񖇂ƃA���t�@�}�b�v�̓������e�N�X�`���z��)
	bool maskRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);

	//�f�v�X�}�b�v���g�p����Ƃ��̃����_�[(���f���̃|�C���^�A�e�s��)
	bool depthRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection);
	bool depthRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection);

	//���C�g�V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃|�C���^�A�A�e�s��A�e�N�X�`���A���C�g�̃p�����[�^�[)	��Deferred�����_�����O�O��
	bool lightRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* LightData);
	bool lightRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* LightData);

	//�V���h�E�V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃|�C���^�A�A�e�s��A�e�N�X�`���A�f�v�X�}�b�v�̃e�N�X�`��)
	bool shadowRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);
	bool shadowRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);

	//�X�|�b�g���C�g�V�F�[�_�[���g�p����Ƃ��̃����_�[
	bool spotRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Light* LightData);

	//�e�N�X�`���V�F�[�_�[���g�p����Ƃ��̃����_�[(���f���̃|�C���^�A�A�e�s��A�e�N�X�`��)
	bool textureRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//�x���V�F�[�f�B���O���g�p����Ƃ��̃����_�[
	bool deferredRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//�}���`�e�N�X�`�������O���g�p����Ƃ��̃����_�[
	bool multiTextureRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum);

	//�X�J�C�h�[����p�����_�[
	bool skyDomeRender(SkyDome* Skydome, Matrix World, Matrix View, Matrix Projection);

	//�X�J�C�h�[���p�_��p�����_�[
	bool skyPlaneRender(Skyplane* Skyplane, Matrix World, Matrix View, Matrix Projection);

	//�e���C����p�����_�[
	bool terrainRender(Terrain* Model, Matrix World, Matrix View, Matrix Projection, Light* Light);

	//�p�[�e�B�N����p�V�F�[�_�[
	bool particleRender(ParticleSystem* ParticleSystem, Matrix World, Matrix View, Matrix Projection);
};

