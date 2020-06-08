#include "stdafx.h"
#include "model.h"
#include"Direct3D.h"
#include"Support.h"
#include"Texture.h"



Model::Model()
{
	ZeroMemory(this, sizeof(Model));
}

Model::~Model()
{
}

bool Model::init(const wchar_t* ModelFileName, MappingType Type,const wchar_t* TextureFileName2)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	wcscpy(texturefilename_, Support::renameToJPEG(ModelFileName));

	loadTexture(texturefilename_);

	if (!initBuffer())
	{
		return false;
	}

	return true;
}

void Model::destroy()
{
	//モデルテクスチャを解放
	releaseTexture();

	SAFE_RELEASE(indexbuff_);
	SAFE_RELEASE(vertexbuff_);

	//モデル解放
	releaseModel();
}

void Model::render()
{
	//グラフィックスパイプラインに頂点バッファをインデックスバッファを配置
	renderBuffer();
}

bool Model::initBuffer()
{
	Vertextype* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexbuffdesc;
	D3D11_BUFFER_DESC indexbuffdesc;
	D3D11_SUBRESOURCE_DATA vertexdata, indexdata;
	HRESULT hr;

	//頂点配列を作成
	vertices = new Vertextype[vertexcount_];
	if (!vertices)
	{
		return false;
	}

	//インデックス配列の作成
	indices = new unsigned long[indexcount_];
	if (!indices)
	{
		return false;
	}

	//頂点配列にデータをロード
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = Vector3(model_[i].x, model_[i].y, model_[i].z);
		vertices[i].texture = Vector2(model_[i].tu, model_[i].tv);
		vertices[i].normal = Vector3(model_[i].nx, model_[i].ny, model_[i].nz);

		indices[i] = i;
	}

	//静的頂点バッファの設定
	vertexbuffdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffdesc.ByteWidth = sizeof(Vertextype) * vertexcount_;
	vertexbuffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbuffdesc.CPUAccessFlags = 0;
	vertexbuffdesc.MiscFlags = 0;
	vertexbuffdesc.StructureByteStride = 0;

	//サブリソースに頂点データへのポインターを与える
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, &vertexbuff_);
	if (FAILED(hr))
	{
		return false;
	}

	//静的インデックスバッファの設定
	indexbuffdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbuffdesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexbuffdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbuffdesc.CPUAccessFlags = 0;
	indexbuffdesc.MiscFlags = 0;
	indexbuffdesc.StructureByteStride = 0;

	//サブリソースにインデックスデータへのポインタを与える
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbuffdesc, &indexdata, &indexbuff_);
	if (FAILED(hr))
	{
		return false;
	}

	//配列を解放
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::renderBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//頂点バッファのストライドとオフセットを設定
	stride = sizeof(Vertextype);
	offset = 0;

	//入力アセンブラで頂点バッファをアクティブにしてレンダリング可能に
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, &vertexbuff_, &stride, &offset);

	//入力アセンブラでインデックスバッファをアクティブにしてレンダリング可能に
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuff_, DXGI_FORMAT_R32_UINT, 0);

	//頂点バッファからレンダリングされるプリミティブのタイプ
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::loadTexture(const wchar_t* FileName)
{

	TextureFactory::getInstance()->getTexture(FileName);
}

bool Model::loadModel(const wchar_t* ModelFileName)
{
	std::fstream fp;
	char input;

	//ファイルパスが有効か確認
	if (!Support::searchFile(ModelFileName))
	{
		return false;
	}

	//ファイルパスが有効なのを確認したので展開
	fp.open(ModelFileName, std::ios::beg | std::ios::in);

	if (fp.fail())
	{
		return false;
	}

	//頂点の数を取得
	fp.get(input);
	while (input != ':')
	{
		fp.get(input);
	}

	//頂点カウントを読み取り
	fp >> vertexcount_;

	//インデックスの数を頂点の数と同じに設定
	indexcount_ = vertexcount_;

	//読み込まれた頂点数を使用してもでるを描画
	model_ = new ModelType[vertexcount_];
	if (!model_)
	{
		return false;
	}

	//データを先頭まで読み取る
	fp.get(input);
	while (input != ':')
	{
		fp.get(input);
	}

	//改行を飛ばす
	fp.get(input);
	fp.get(input);

	//頂点データを読み込み
	for (int i = 0; i < vertexcount_; i++)
	{
		fp >> model_[i].x >> model_[i].y >> model_[i].z;
		fp >> model_[i].tu >> model_[i].tv;
		fp >> model_[i].nx >> model_[i].ny >> model_[i].nz;
	}

	fp.close();

	return true;
}

void Model::releaseTexture()
{
	TextureFactory::getInstance()->deleteTexture(texturefilename_);
}

void Model::releaseModel()
{
	if (model_)
	{
		delete[] model_;
		model_ = nullptr;
	}
}

void Model::setPosition(const float X, const float Y, const float Z)
{
	positionx_ = X;
	positiony_ = Y;
	positionz_ = Z;
}

void Model::getPosition(float& X, float& Y, float& Z)
{
	X = positionx_;
	Y = positiony_;
	Z = positionz_;
}
