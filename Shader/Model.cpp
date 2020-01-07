#include "stdafx.h"
#include "model.h"
#include"Direct3D.h"
#include"TextureFactory.h"
bool Model::initbuff()
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
	//ZeroMemory(&vertexdata, sizeof(vertexdata));
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

void Model::destroybuff()
{
	//インデックスバッファを解放
	if (indexbuff_)
	{
		indexbuff_->Release();
		indexbuff_ = nullptr;
	}

	//頂点バッファの解放
	if (vertexbuff_)
	{
		vertexbuff_->Release();
		vertexbuff_ = nullptr;
	}
}

void Model::renderbuff()
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

bool Model::Loadtexture(const wchar_t* FileName)
{
	memcpy(filename_, FileName, sizeof(wchar_t*));
	texture_ = TextureFactory::getInstance()->getTexture(FileName);

	return true;
}

void Model::Releasetexture()
{
	TextureFactory::getInstance()->deleteTexture(filename_);
}

bool Model::LoadModel(const wchar_t* FileName)
{
	std::ifstream fin;
	char input;

	//ファイル展開
	fin.open(FileName);

	//展開失敗
	if (fin.fail())
	{
		return false;
	}

	//頂点の数を取得
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//頂点カウントを読み取り
	fin >> vertexcount_;

	//インデックスの数を頂点の数と同じに設定
	indexcount_ = vertexcount_;

	//読み込まれた頂点数を使用してもでるを描画
	model_ = new ModelType[vertexcount_];
	if (!model_)
	{
		return false;
	}

	//データを先頭まで読み取る
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//頂点データを読み込み
	for (int i = 0; i < vertexcount_; i++)
	{
		fin >> model_[i].x >> model_[i].y >> model_[i].z;
		fin >> model_[i].tu >> model_[i].tv;
		fin >> model_[i].nx >> model_[i].ny >> model_[i].nz;
	}

	fin.close();

	return true;
}

void Model::ReleaseModel()
{
	if (model_)
	{
		delete model_;
		model_ = nullptr;
	}
}

Model::Model()
{
	vertexbuff_ = nullptr;
	indexbuff_ = nullptr;
	model_ = nullptr;
}

Model::~Model()
{
}

bool Model::init(const wchar_t* TextureFileName, const wchar_t* ModelFileName)
{
	bool result;
	//モデルデータ読み込み
	result = LoadModel(ModelFileName);
	if (!result)
	{
		return false;
	}

	//頂点及びインデックスバッファを初期化
	result = initbuff();

	//頂点とインデックスバッファを初期化
	if (!result)
	{
		return false;
	}

	//このモデルで使用するテクスチャをロード
	result = Loadtexture(TextureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::destroy()
{
	//モデルテクスチャを解放
	Releasetexture();

	//頂点バッファとインデックスバッファを破棄
	destroybuff();

	//モデル解放
	ReleaseModel();
}

void Model::render()
{
	//グラフィックスパイプラインに頂点バッファをインデックスバッファを配置
	renderbuff();
}

void Model::setPosition(const float X, const float Y, const float Z)
{
	positionx = X;
	positiony = Y;
	positionz = Z;
}

void Model::getPosition(float& X, float& Y, float& Z)
{
	X = positionx;
	Y = positiony;
	Z = positionz;
}
