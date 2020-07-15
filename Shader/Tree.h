#pragma once
#include"TextureFactory.h"
#include"Support.h"
#include "Direct3D.h"

class Tree
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
		Vector3 normal;
	};

	struct ModelType
	{
		Vector3 position;
		Vector2 texpos;
		Vector3 normalpos;
	};
	Direct3D* instanceptr_;
	ComPtr<ID3D11Buffer> trunkvertexbuffer_;
	ComPtr<ID3D11Buffer> trunkindexbuffer_;
	ComPtr<ID3D11Buffer> leafvertexbuffer_;
	ComPtr<ID3D11Buffer> leafindexbuffer_;
	int vertexcount_;
	int indexcount_;
	int trunkindexcount_;
	int leafindexcount_;
	float scale_;
	wchar_t trunkfilename_[MAX_PATH];
	wchar_t leaffilename_[MAX_PATH];
	Vector3 position_;
	std::vector<ModelType> model_;

	bool initTrunkBuffer();
	bool initLeafBuffer();
	void renderTrunkBuffer();
	void renderLeavesBuffer();
	bool loadTexture();
	void releaseTexture();
	bool loadModel(const wchar_t* ModelName);
	void releaseModel();
	void destroyBuffer();
	

public:
	Tree();
	~Tree();
	bool init(const wchar_t* TrunkModelFileName, const wchar_t* LeafModelFileName,float Scale);
	void renderTrunk();
	void renderLeaves();
	void destroy();

	//get
	inline const int getTrunkIndexCount()const { return trunkindexcount_; } //�؂̊��̃��f���̃C���f�b�N�X�J�E���g��Ԃ�
	inline const int getLeafIndexCount()const { return leafindexcount_; } //�؂̗t�̃��f���̃C���f�b�N�X�J�E���g��Ԃ�
	inline const Vector3 getPosition()const { return position_; } //���W��Ԃ�
	inline ID3D11ShaderResourceView* getTrunkTexture() { return TextureFactory::getInstance()->getTexture(trunkfilename_); }	//�؂̊��̃e�N�X�`����Ԃ�
	inline ID3D11ShaderResourceView* getLeafTexture() { return TextureFactory::getInstance()->getTexture(leaffilename_); }     //�؂̗t�̃e�N�X�`����Ԃ�

	//set
	inline void setPosition(const Vector3& Position) { position_ = Position; } //���W��ݒ�

};

