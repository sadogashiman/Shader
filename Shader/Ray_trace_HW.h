#pragma once
#include"Support.h"
#include"Light.h"
class Ray_trace_HW
{
private:
	struct StructuredElement
	{
		 Vector3 startpoint;
		 Vector3 direction;
		 Vector3 intersection;
		 Vector3 normal;
		 float distance;
		 DWORD color;
	};

	struct ConstantBufferType
	{
		ALIGN16 Vector3 position;
		ALIGN16 float r; //���a
		ALIGN16 Matrix sphereiw; //���̃��[���h�t�s��
		ALIGN16 Matrix miw; //�J�����t�s��
		ALIGN16 Vector3 lightposition; //���C�g���W
	};

	struct Sphere
	{
		Vector3 position; //���S���W
		float r; //���a
		Matrix im; //���[���h�̋t�s��
	};

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11ComputeShader* computeshader_;
	ID3D11InputLayout* layout_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	ComPtr<ID3D11Texture2D> cpdynamictexture_;
	ID3D11ShaderResourceView* dynamictexture_;
	Vector3 eyepos_;
	unsigned int numsphere_;
	Sphere* spheresrray_[100];


	std::unique_ptr<Support> support_;

	//�R���s���[�g�V�F�[�_�[�p
	ComPtr<ID3D11Buffer> cpstructuredbuffer_;
	ComPtr<ID3D11Buffer> cpbufferinput_;       //�V�F�[�_�[���̃��C�f�[�^�Q�Ɨp
	ComPtr<ID3D11Buffer> cpbufferresult_;	   //�V�F�[�_�[����̌��ʂ��󂯎��p
	ComPtr<ID3D11Buffer> cpconstantbuffer_;	   //���C��n�����߂̃o�b�t�@
	ComPtr<ID3D11ShaderResourceView> cpbufferinputsrv_; //�V�F�[�_�[���̃��C�f�[�^�Q�Ɨp
	ComPtr<ID3D11UnorderedAccessView> cpbufferresultuav_; //���ʂ��󂯎�邽�߂̕ϐ�


	//�֐�
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection,Light* Light);
	void renderShader(const int IndexCount);
	Vector3 transScreenToWorld(const int ScreenHeight, const int ScreenWidth); //�X�N���[�����W�����[���h���W�ɕϊ�
	void addSphere(Sphere* Sp);
public:
	Ray_trace_HW();
	~Ray_trace_HW();
	bool init();
	bool render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Light* Light);
	void destroy();
};

