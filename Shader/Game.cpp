#include "stdafx.h"
#include "Game.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"ShaderManager.h"

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

	result = model_->init(L"Resource/cube.txt");
	if (!result)
	{
		return false;
	}

	//�S��ʂ�2D�E�B���h�E�𐶐�
	ortho_ = new OrthoWindow;
	if (!ortho_)
	{
		return false;
	}

	result = ortho_->init(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
	if (!result)
	{
		return false;
	}

	//�o�b�t�@���쐬
	defbuffer_ = new Deferredbuffers;
	if (!defbuffer_)
	{
		return false;
	}

	if (!defbuffer_->init(ScreenWidth, ScreenHeight, kScreen_depth, kScreen_near))
	{
		return false;
	}

	raytrace_ = new RayTrace;
	Sphere sp;
	sp.position = Vector3::Zero;
	sp.rdius = 0.4F;

	raytrace_->addSphere(&sp);

	rayhw_ = new Ray_trace_HW;
	if (!rayhw_->init())
	{
		return false;
	}


	return true;

}

bool Game::update()
{
	bool result;
	result = rayhw_->render();
	//result = raytrace_->render();

	//result = render();
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
	static float rotation = 0;

	if (rotation > 360.0F)
	{
		rotation -= 360.0F;
	}
	else
	{
		rotation++;
	}


	////�V�[���������_�[�o�b�t�@�[�Ƀ����_�����O
	//result = renderSceneToTexture();
	//if (!result)
	//{
	//	return false;
	//}

	//�V�[�����N���A
	Direct3D::getInstance()->begin(Colors::Black);

	//�s����擾
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	ortho = Direct3D::getInstance()->getOrtho();
	view = camera_->getBaseViewMatrix();
	world = XMMatrixRotationY(XMConvertToRadians(rotation));

	//Direct3D::getInstance()->turnZbufferOff();

	//�t���X�N���[����2D�E�B���h�E���쐬
	//ortho_->render();
	if (!(ShaderManager::getInstance()->textureRender(model_, world, view, projection,model_->getTexture())))
	{
		return false;
	}

	//if (!(ShaderManager::getInstance()->lightRender(ortho_->getIndexCount(),world,view,ortho,defbuffer_->getShaderResourceView(0),defbuffer_->getShaderResourceView(1),light_)))
	//{
	//	return false;
	//}

	////���ׂĂ�2D�����_�����O���I�������̂�Z�o�b�t�@��L���ɂ���
	//Direct3D::getInstance()->turnZbufferOn();

	//�`��I��
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(ortho_);
	SAFE_DELETE_DESTROY(model_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
	rayhw_->destroy();

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