#include "stdafx.h"
#include "model.h"
#include"Support.h"
#include"Texture.h"



Model::Model()
{
	ZeroMemory(&texturefilename_, sizeof(texturefilename_));
	ZeroMemory(&mapfilename_, sizeof(mapfilename_));
	scale_ = 1.0F;
	world_ = XMMatrixIdentity();
	instanceptr_ = Direct3D::getInstance();
}

Model::~Model()
{
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//ファイル名を保存
	wcscpy(texturefilename_[0], TextureFileName);

	if (!initBuffer())
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}

	return true;
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//ファイル名を保存
	wcscpy(texturefilename_[0], TextureFileName1);
	wcscpy(texturefilename_[1], TextureFileName2);

	if (!initBuffer())
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}

	return true;
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2, const wchar_t* MapFileName)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//ファイル名を保存
	wcscpy(texturefilename_[0], TextureFileName1);
	wcscpy(texturefilename_[1], TextureFileName2);
	wcscpy(mapfilename_, MapFileName);

	if (!initBuffer())
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}

	return true;
}

void Model::destroy()
{
	//このモデルが使用したテクスチャを解放させる
	for (int i = 0; i < kMaxTexture; i++)
	{
		if (texturefilename_[i] != NULL)
			TextureFactory::getInstance()->deleteTexture(texturefilename_[0]);
	}

	if (mapfilename_ != NULL)
	{
		TextureFactory::getInstance()->deleteTexture(mapfilename_);
	}

}

void Model::render()
{
	//グラフィックスパイプラインに頂点バッファをインデックスバッファを配置
	renderBuffer();
}

Matrix Model::getWorldMatrix()
{
	world_ =  Matrix::Identity;
	world_ *= Matrix::CreateScale(scale_);
	world_ *= Matrix::CreateRotationX(XMConvertToRadians(rotation_.x));
	world_ *= Matrix::CreateRotationY(XMConvertToRadians(rotation_.y));
	world_ *= Matrix::CreateRotationZ(XMConvertToRadians(rotation_.z));
	world_ *= Matrix::CreateTranslation(position_);

	return world_;
}

bool Model::initBuffer()
{
	std::vector<Vertextype> vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbuffdesc;
	D3D11_BUFFER_DESC indexbuffdesc;
	D3D11_SUBRESOURCE_DATA vertexdata, indexdata;
	HRESULT hr;

	//頂点配列を作成
	vertices.resize(vertexcount_);

	//インデックス配列の作成
	indices.resize(indexcount_);

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
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, vertexbuff_.GetAddressOf());
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
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&indexbuffdesc, &indexdata, indexbuff_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

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
	instanceptr_->getContext()->IASetVertexBuffers(0, 1, vertexbuff_.GetAddressOf(), &stride, &offset);

	//入力アセンブラでインデックスバッファをアクティブにしてレンダリング可能に
	instanceptr_->getContext()->IASetIndexBuffer(indexbuff_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//頂点バッファからレンダリングされるプリミティブのタイプ
	instanceptr_->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::loadModel(const wchar_t* ModelFileName)
{
	std::fstream fs;
	char input;
	FILE* fp;
	char tmp[MAX_PATH];
	char buffer[256];

	//ファイルパスが有効か確認
	if (!Support::searchFile(ModelFileName))
	{
		return false;
	}

	//ファイル展開のために文字列の型変換
	wcstombs(tmp, ModelFileName,MAX_PATH);

	//ファイル展開
	fp = fopen(tmp,"r");
	if (fp == nullptr)
		return false;

	//ファイル読み込み
	input = fgetc(fp);

	//頂点数まで飛ばす
	while (input != ':')
	{
		input = fgetc(fp);
	}

	//頂点数を取得
	fscanf(fp, "%d", &vertexcount_);

	//インデックス数を設定
	indexcount_ = vertexcount_;

	//配列サイズを変更
	model_.resize(vertexcount_);

	input = fgetc(fp);

	//先頭まで読み飛ばす
	while (input != ':')
	{
		input = fgetc(fp);
	}

	//改行を飛ばす
	fgets(buffer, 256, fp);

	//モデルパラメーター取得
	for (int i = 0; i < vertexcount_; i++)
	{
		//座標を取得
		fscanf(fp, "%f %f %f",&model_[i].x, &model_[i].y, &model_[i].z);

		//テクスチャUVを取得
		fscanf(fp, "%f %f", &model_[i].tu, &model_[i].tv);

		//法線を取得
		fscanf(fp, "%f %f %f", &model_[i].nx, &model_[i].ny, &model_[i].nz);
	}

	//ファイルクローズ
	fclose(fp);

	return true;
}
