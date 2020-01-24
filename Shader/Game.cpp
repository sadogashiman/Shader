#include "stdafx.h"
#include "Game.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"Renderer.h"

Game::Game()
{
	light_ = nullptr;
	lightshader_ = nullptr;
	model_ = nullptr;
	camera_ = nullptr;
	ortho_ = nullptr;
	defbuffer_ = nullptr;
}

Game::~Game()
{
}

bool Game::init(HWND Hwnd, const int ScreenWidth, const int ScreenHeight)
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

	//���f���I�u�W�F�N�g�𐶐�
	model_ = new Model;
	if (!model_)
	{
		return false;
	}

	result = model_->init(L"Resource/stone01.dds",L"Resource/model.txt",kBumpMap);
	if (!result)
	{
		return false;
	}

	//�S��ʂ�2D�E�B���h�E�𐶐�
	//ortho_ = new OrthoWindow;
	//if (!ortho_)
	//{
	//	return false;
	//}

	//result = ortho_->init(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
	//if (!result)
	//{
	//	return false;
	//}

	//�o�b�t�@���쐬
	//defbuffer_ = new Deferredbuffers;
	//if (!defbuffer_)
	//{
	//	return false;
	//}

	//if (!defbuffer_->init(ScreenWidth, ScreenHeight, kScreen_depth, kScreen_near))
	//{
	//	return false;
	//}

	return true;
}

bool Game::update()
{
	bool result;
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Game::render()
{
	bool result;
	Matrix world, view, projection,ortho;

	////�V�[���������_�[�o�b�t�@�[�Ƀ����_�����O
	//result = renderSceneToTexture();
	//if (!result)
	//{
	//	return false;
	//}

	//�V�[�����N���A
	Direct3D::getInstance()->begin(Colors::Black);

	//�s����擾
	Direct3D::getInstance()->getWorld(world);
	Direct3D::getInstance()->getProjection(projection);
	Direct3D::getInstance()->getOrtho(ortho);
	view = camera_->getBaseViewMatrix();

	//Direct3D::getInstance()->turnZbufferOff();

	//�t���X�N���[����2D�E�B���h�E���쐬
	//ortho_->render();

		//���f������]
	static float rotation = 0.0F;
	rotation += static_cast<float>(XM_PI * 0.005F);
	if (rotation > 360.0F)
	{
		rotation -= 360;
	}

	world = XMMatrixRotationY(rotation);

	model_->render();

	if (!(Renderer::getInstance()->textureRender(model_->getIndexCount(),world,view,projection,model_->getTexture())))
	{
		return false;
	}

	//���ׂĂ�2D�����_�����O���I�������̂�Z�o�b�t�@��L���ɂ���
	//Direct3D::getInstance()->turnZbufferOn();

	//�`��I��
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	//SAFE_DELETE_DESTROY(defbuffer_);
	//SAFE_DELETE_DESTROY(ortho_);
	SAFE_DELETE_DESTROY(model_);
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
	Direct3D::getInstance()->getWorld(world);
	Direct3D::getInstance()->getProjection(projection);
	view = camera_->getViewMatrix();



	//���f���������_�����O
	model_->render();

	if (!(Renderer::getInstance()->deferredRender(model_->getIndexCount(), world, view, projection, model_->getTexture())))
	{
		return false;
	}

	//�o�b�N�o�b�t�@�Ƀ����_�����O�^�[�Q�b�g���ڂ�
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//�r���[�|�[�g�����Z�b�g
	Direct3D::getInstance()->resetViewPort();

	return true;
}