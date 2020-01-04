#include "stdafx.h"
#include "Application.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"Renderer.h"

Application::Application()
{
	light_ = nullptr;
	lightshader_ = nullptr;
	model_ = nullptr;
	camera_ = nullptr;
	ortho_ = nullptr;
	defbuffer_ = nullptr;
	defshader_ = nullptr;
}

Application::~Application()
{
}

bool Application::init(HWND Hwnd, const int ScreenWidth, const int ScreenHeight)
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

	result = model_->init(L"Resource/seafloor.dds",L"Resource/cube.txt");
	if (!result)
	{
		return false;
	}

	//�S��ʂ̃I���\�E�B���h�E�𐶐�
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

	//�x���o�b�t�@�I�u�W�F�N�g�𐶐�
	defbuffer_ = new Deferredbuffers;
	if (!defbuffer_)
	{
		return false;
	}

	//�x���o�b�t�@�I�u�W�F�N�g��������
	result = defbuffer_->init(ScreenWidth, ScreenHeight, kScreen_depth, kScreen_near);
	if (!result)
	{
		return false;
	}
	defshader_ = new Deferredshader;
	if (!defshader_)
	{
		return false;
	}

	//�x���V�F�[�_�[�I�u�W�F�N�g�𐶐�
	result = defshader_->init();
	if (!result)
	{
		return false;
	}

	//���C�g�V�F�[�_�[�I�u�W�F�N�g�𐶐�
	lightshader_ = new LightShader;
	if (!result)
	{
		return false;
	}

	result = lightshader_->init();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::update()
{
	return true;
}

bool Application::render()
{
	bool result;
	Matrix world, baseview, ortho;

	//�V�[���������_�[�o�b�t�@�[�Ƀ����_�����O
	result = renderSceneToTexture();
	if (!result)
	{
		return false;
	}

	//�V�[�����N���A
	Direct3D::getInstance()->begin(Colors::Black);

	//�s����擾
	Direct3D::getInstance()->getWorld(world);
	Direct3D::getInstance()->getOrtho(ortho);
	baseview = camera_->getBaseViewMatrix();

	//2D�����_�����O�J�n
	Direct3D::getInstance()->turnZbufferOff();

	ortho_->render();

	//�t���X�N���[����2D�E�B���h�E���쐬
	Renderer::getInstance()->lightRender(ortho_->getIndexCount(), world, baseview, ortho, defbuffer_->getShaderResourceView(0), defbuffer_->getShaderResourceView(1), light_);

	//���ׂĂ�2D�����_�����O���I�������̂�Z�o�b�t�@��L���ɂ���
	Direct3D::getInstance()->turnZbufferOn();

	//�`��I��
	Direct3D::getInstance()->end();

	return true;
}

void Application::destroy()
{
	SAFE_DELETE_DESTROY(lightshader_);
	SAFE_DELETE_DESTROY(defshader_);
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(ortho_);
	SAFE_DELETE_DESTROY(model_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Application::renderSceneToTexture()
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

	defshader_->render(model_->getIndexCount(), world, view, projection, model_->getTexture());

	//�o�b�N�o�b�t�@�Ƀ����_�����O�^�[�Q�b�g���ڂ�
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//�r���[�|�[�g�����Z�b�g
	Direct3D::getInstance()->resetViewPort();

	return true;
}