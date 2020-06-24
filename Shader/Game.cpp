#include "stdafx.h"
#include "Game.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"ShaderManager.h"
#include"System.h"
#include"Timer.h"
#include"SkyDome.h"


Game::Game()
{
}

Game::~Game()
{
}

bool Game::init()
{
	bool result;

	//�J�����I�u�W�F�N�g�𐶐�
	camera_ = new Camera;
	if (!camera_)
	{
		return false;
	}

	result = camera_->init();
	if (!result)
	{
		Error::showDialog("�J�����̏������Ɏ��s");
		return false;
	}

	camera_->setPosition(Vector3(0.0F, 3.0F, 10.0F));
	camera_->render();
	camera_->renderBaseViewMatrix();

	//���C�g�I�u�W�F�N�g�𐶐�
	light_ = new Light;
	if (!light_)
	{
		return false;
	}

	light_->setAmbientColor(0.05F, 0.05F, 0.05F, 1.0F);
	light_->setDiffuseColor(1.0F, 1.0F, 1.0F, 1.0F);
	light_->setDirection(-0.5F, -1.0F, 0.0F);

	sky_ = new SkyDome;
	if (!sky_)
	{
		return false;
	}

	result = sky_->init(L"Resource/skydome.txt");
	if (!result)
	{
		return false;
	}

	sky_->setApexColor(0.0f, 0.05f, 0.6f, 1.0f);
	sky_->setCentorColor(0.0f, 0.5f, 0.8f, 1.0f);

	terrain_ = new Terrain;
	if (!terrain_)
	{
		return false;
	}

	result = terrain_->init(L"Resource/setup.txt", L"Resource/dirt01.dds");
	if (!result)
	{
		return false;
	}

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	position_.setPosition(Vector3(128.0F, 10.0F, -10.0F));
	position_.setRotation(Vector3::Zero);

	skyplane_ = new Skyplane;
	if (!skyplane_)
	{
		return false;
	}

	result = skyplane_->init(L"Resource/cloud001.dds", L"Resource/perturb001.dds");
	if (!result)
	{
		Error::showDialog("skyplane�̏������Ɏ��s");
		return false;
	}
	wire_ = false;

	return true;
}

State* Game::update()
{
	bool result;
	camera_->update();
	switchWireFrame();

	result = render();
	if (!result)
	{
		return nullptr;
	}


	return this;
}

bool Game::render()
{


	//�V�[�����N���A
	Direct3D::getInstance()->begin(Colors::CornflowerBlue);

	if (!worldRender())
	{
		return false;
	}


	

	//�`��I��
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE_DESTROY(skyplane_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

void Game::switchWireFrame()
{
	if (Input::getInstance()->isKeyPressed(DIK_F2))
	{
		if (wire_)
		{
			wire_ = false;
		}
		else
		{
			wire_ = true;
		}
	}
}

bool Game::modelRender()
{
	return true;
}

bool Game::worldRender()
{
	Matrix world, projection;
	Matrix skyworld;
	Matrix view;

	//�s����擾
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	skyworld = XMMatrixTranslation(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	//�J�������_�������_�����O
	camera_->render();

	//Z�o�b�t�@�E�J�����O���I�t
	Direct3D::getInstance()->turnCullingDisable();
	Direct3D::getInstance()->turnZbufferDisable();

	//�X�J�C�h�[���������_�����O
	if (!(ShaderManager::getInstance()->skyDomeRender(sky_, skyworld, view, projection)))
	{
		return false;
	}

	//�J�����O���I��
	Direct3D::getInstance()->turnCullingEnable();

	//���Z�����I��
	Direct3D::getInstance()->turnAddBlendEnable();

	//�_�������_�����O
	if (!(ShaderManager::getInstance()->skyPlaneRender(skyplane_, skyworld, view, projection)))
	{
		return false;
	}

	//�����I�t
	Direct3D::getInstance()->turnAlphablendDisable();

	//Z�o�b�t�@�I��
	Direct3D::getInstance()->turnZbufferEnable();
	//���C���[�t���[���؂�ւ�

	if (wire_)
		Direct3D::getInstance()->wireFrameEnable();

	//�e���C���������_�����O
	if (!(ShaderManager::getInstance()->terrainRender(terrain_, world, view, projection, light_)))
	{
		return false;
	}

	if (wire_)
		Direct3D::getInstance()->wireFrameDisable();

	//2D�����_�����O���I�������̂�Z�o�b�t�@��L���ɂ���
	Direct3D::getInstance()->turnZbufferEnable();

	return true;
}
