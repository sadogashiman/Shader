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

	orthowindow_ = new OrthoWindow;
	if (!orthowindow_)
	{
		return false;
	}

	result = orthowindow_->init(kWindow_Width, kWindow_Height);
	if (!result)
	{
		Error::showDialog("2D�E�B���h�E�̏������Ɏ��s");
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
	Matrix world, projection,ortho;
	Matrix baseview;
	result = renderSceneToTexture();
	if (!result)
	{
		return false;
	}

	//�V�[�����N���A
	Direct3D::getInstance()->begin(Colors::CornflowerBlue);

	//�s����擾
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	baseview = camera_->getBaseViewMatrix();
	ortho = Direct3D::getInstance()->getOrtho();
	
	//�����_�����O��2D�ōs���̂�Z�o�b�t�@�𖳌��ɂ���
	Direct3D::getInstance()->turnZbufferOff();

	//�`��p��2D�E�B���h�E������
	orthowindow_->render();

	result = ShaderManager::getInstance()->lightRender(orthowindow_->getIndexCount(), world, baseview, ortho,defbuffer_->getShaderResourceView(0),defbuffer_->getShaderResourceView(1),light_);
	if (!result)
	{
		return false;
	}

	//2D�����_�����O���I�������̂�Z�o�b�t�@��L���ɂ���
	Direct3D::getInstance()->turnZbufferOn();



	//�`��I��
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE_DESTROY(orthowindow_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Game::renderSceneToTexture()
{
	Matrix world, view, projection;
	Matrix skyworld;

	//�����_�[�^�[�Q�b�g���쐬
	defbuffer_->setRenderTargets();

	//�����_�[�o�b�t�@���N���A
	defbuffer_->clearRenderTargets(Colors::Black);

	//�s����擾
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	skyworld = XMMatrixTranslation(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	camera_->render();

	Direct3D::getInstance()->turnCullingOff();
	Direct3D::getInstance()->turnZbufferOff();

	if (!(ShaderManager::getInstance()->skyDomeRender(sky_, skyworld, view, projection)))
	{
		return false;
	}

	Direct3D::getInstance()->turnZbufferOn();
	Direct3D::getInstance()->turnCullingOn();

	if (wire_)
		Direct3D::getInstance()->wireFrameEnable();

	if (!(ShaderManager::getInstance()->deferredRender(terrain_, world, view, projection, terrain_->getTexture())))
	{
		return false;
	}

	if (wire_)
		Direct3D::getInstance()->wireFrameDisable();

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

	position_.turnLeft(Input::getInstance()->isKeyState(DIK_LEFT));
	position_.turnRight(Input::getInstance()->isKeyState(DIK_RIGHT));
	position_.moveForWard(Input::getInstance()->isKeyState(DIK_UP));
	position_.moveBackWard(Input::getInstance()->isKeyState(DIK_DOWN));
	position_.moveUpWard(Input::getInstance()->isKeyState(DIK_A));
	position_.moveDownWard(Input::getInstance()->isKeyState(DIK_Z));
	position_.lookUpWard(Input::getInstance()->isKeyState(DIK_PGUP));
	position_.lookDownWard(Input::getInstance()->isKeyState(DIK_PGDN));
	
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
