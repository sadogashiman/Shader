#include "stdafx.h"
#include "model.h"
#include"Direct3D.h"
#include"Support.h"

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
		vertices[i].position = Vector3(vertexvec_[i].x, vertexvec_[i].y, vertexvec_[i].z);
		vertices[i].texture = Vector2(texturevec_[i].x,texturevec_[i].y);
		vertices[i].normal = Vector3(normalvec_[i].x, normalvec_[i].y, normalvec_[i].z);

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
	hr =Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, &vertexbuff_);
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
	//メンバに文字列を保存
	wcscpy(texturefilename_, FileName);

	TextureFactory::getInstance()->getTexture(FileName);
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

Model::Model()
{
	vertexbuff_ = nullptr;
	indexbuff_ = nullptr;
	model_ = nullptr;
	ZeroMemory(texturefilename_ ,sizeof(texturefilename_));
	vertexcount_ = 0;
	indexcount_ = 0;
	positionx_ = 0.0F;
	positiony_ = 0.0F;
	positionz_ = 0.0F;
}

Model::~Model()
{
}

bool Model::init(const wchar_t* TextureFileName, const wchar_t* ModelFileName, MappingType Type,const wchar_t* TextureFileName2)
{
	bool result;
	//ファイルパスが有効か確認
	if (!Support::searchFile(ModelFileName))
	{
		return false;
	}

	if (!Support::searchFile(TextureFileName))
	{
		Error::showDialog("モデルテクスチャファイル名が無効です");
		return false;
	}
	else
	{
		loadTexture(TextureFileName);
	}
	std::fstream fp;

	//ファイル展開
	fp.open(ModelFileName, std::ios::in|std::ios::beg);

	std::string tmp;
	//データの読み込み
	while (!fp.eof())
	{
		fp >> tmp;
		if (tmp=="#")
		{
			//コメントは無視
			continue;
		}
		else if (tmp=="vn")
		{
			Vector3 npos;
			fp >> npos.x >> npos.y >> npos.z;
			npos.z *= -1.0F;
			normalvec_.push_back(npos);
		}
		else if (tmp == "vt")
		{
			Vector2 texpos;
			fp >> texpos.x >> texpos.y;
			texpos.y *= -1.0F;
			texturevec_.push_back(texpos);
		}
		else if (tmp == "v")
		{
			Vector3 vpos = Vector3::Zero;
			fp >> vpos.x >> vpos.y >> vpos.z;
			vpos.z *= -1.0F; //座標系を反転
			vertexvec_.push_back(vpos);
			vertexcount_++;
		}
		else if (tmp == "f")
		{
			unsigned int value;
			Vertextype vertex;
			
			for (int i = 0; i < 3; i++)
			{
				fp >> value;
				vertex.position = vertexvec_[value - 1];
				fp.ignore();

				fp >> value;
				vertex.texture = texturevec_[value - 1];
				fp.ignore();

				fp >> value;
			}
		}
	}

	indexcount_ = vertexcount_;

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
