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
	camera_->setPosition(Vector3(0.0F, 0.0F, -10.0F));
	camera_->render();
	camera_->renderBaseViewMatrix();

	//���C�g�I�u�W�F�N�g�𐶐�
	light_ = new Light;
	if (!light_)
	{
		return false;
	}

	light_->setDiffuseColor(1.0F, 1.0F, 1.0F, 1.0F);
	light_->setDirection(0.0F, 0.0F, 1.0F);

	//�o�b�t�@���쐬
	defbuffer_ = new Deferredbuffers;
	if (!defbuffer_)
	{
		return false;
	}

	if (!defbuffer_->init(System::getWindowWidth(), System::getWindowHeight(), kScreen_depth, kScreen_near))
	{
		return false;
	}

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

	result = terrain_->init(L"Resource/setup.txt", L"Resource/dirt.tga");
	if (!result)
	{
		return false;
	}

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	position_.setPosition(Vector3(128.0F, 10.0F, -10.0F));
	position_.setRotation(Vector3::Zero);

	wire_ = false;

	return true;

}

State* Game::update()
{
	bool result;
	handleMovementInput();

	result = render();
	if (!result)
	{
		return nullptr;
	}


	return this;
}

bool Game::render()
{
	bool result;
	Matrix world, view, projection,ortho;
	Matrix baseview;
	static float rotation = 0;

	if (rotation < 360.0F)
	{
		rotation += 0.05F;
	}


	camera_->render();

	//�V�[�����N���A
	Direct3D::getInstance()->begin(Colors::CornflowerBlue);

	//�s����擾
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	baseview = camera_->getBaseViewMatrix();
	if(wire_)
	Direct3D::getInstance()->wireFrameEnable();

	result = ShaderManager::getInstance()->textureRender(terrain_, world, view, projection,terrain_->getTexture());
	if (!result)
	{
		return false;
	}

	if (wire_)
	Direct3D::getInstance()->wireFrameDisable();

	//�`��I��
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Game::renderSceneToTexture()
{
	Matrix world, view, projection;

	//�����_�[�^�[�Q�b�g���쐬
	defbuffer_->setRenderTargets();

	//�����_�[�o�b�t�@���N���A
	defbuffer_->clearRenderTargets(Colors::Black);

	//�s����擾
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();

	//���f������]
	static float rotation = 0.0F;
	rotation += static_cast<float>(XM_PI * 0.005F);
	if (rotation > 360.0F)
	{
		rotation -= 360;
	}

	world = XMMatrixRotationY(rotation);

	//���f���������_�����O
	model_->render();

	if (!(ShaderManager::getInstance()->deferredRender(model_, world, view, projection, model_->getTexture())))
	{
		return false;
	}

	//�o�b�N�o�b�t�@�Ƀ����_�����O�^�[�Q�b�g���ڂ�
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//�r���[�|�[�g�����Z�b�g
	Direct3D::getInstance()->resetViewPort();

	return true;
}

void Game::handleMovementInput()
{
	//�J��������
	position_.setFrameTime(16.0F);

	position_.turnLeft(Input::getInstance()->keyDown(DIK_LEFT));
	position_.turnRight(Input::getInstance()->keyDown(DIK_RIGHT));
	position_.moveForWard(Input::getInstance()->keyDown(DIK_UP));
	position_.moveBackWard(Input::getInstance()->keyDown(DIK_DOWN));
	position_.moveUpWard(Input::getInstance()->keyDown(DIK_A));
	position_.moveDownWard(Input::getInstance()->keyDown(DIK_Z));
	position_.lookUpWard(Input::getInstance()->keyDown(DIK_PGUP));
	position_.lookDownWard(Input::getInstance()->keyDown(DIK_PGDN));
	
	camera_->setPosition(position_.getPosition());
	camera_->setRotation(position_.getRotation());
	Input::getInstance()->anyKeyUp();
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
