#include "stdafx.h"
#include "Skyplane.h"
#include "Support.h"
#include "Direct3D.h"

Skyplane::Skyplane()
{
}

Skyplane::~Skyplane()
{
}

bool Skyplane::init(const wchar_t* TextureFileName1, const wchar_t* TextureFileName2)
{
	int skyresolution;
	int texrepeat;
	float planewidth;
	float planetop;
	float planebottom;
	bool result;
	std::vector<VertexType>vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA indexdata;
	D3D11_SUBRESOURCE_DATA vertexdata;
	HRESULT hr;
	int index;
	int index1;
	int index2;
	int index3;
	int index4;

	//パラメーターの設定
	scale_ = 0.5F;
	bright_ = 0.5F;

	//変化量をゼロに
	transition_ = 0.0F;

	//テクスチャファイル名をメンバに保存
	wcscpy(cloudtexfilename_, TextureFileName1);
	wcscpy(perturbtexfilename_, TextureFileName2);

	//各パラメータ設定
	skyresolution = 10;
	planewidth = 10.0F;
	planetop = 0.5F;
	planebottom = 0.0F;
	texrepeat = 4;

	//平面を作成
	result = initskyPlane(skyresolution, planewidth, planetop, planebottom, texrepeat);
	if (!result)
	{
		Error::showDialog("平面の作成に失敗");
		return false;
	}

	//バッファの初期化
	vertexcount_ = (skyresolution + 1) * (skyresolution + 1) * 6;

	//インデックスカウントを頂点数と同じにする
	indexcount_ = vertexcount_;

	//頂点配列を作成
	vertices.resize(vertexcount_);

	//インデックス配列を作成
	indices.resize(indexcount_);

	//インデックスを初期化
	index = 0;

	//頂点とインデックス配列を平面データと一緒にロード
	for (int i = 0; i < skyresolution; i++)
	{
		for (int j = 0; j < skyresolution; j++)
		{
			index1 = i * (skyresolution + 1) + j;
			index2 = i * (skyresolution + 1) + (j + 1);
			index3 = (i + 1) * (skyresolution + 1) + j;
			index4 = (i + 1) * (skyresolution + 1) + (j + 1);

			//左上
			vertices[index].position = skyplane_[index1].position;
			vertices[index].texture = skyplane_[index1].texture;
			indices[index] = index;
			index++;

			//右上
			vertices[index].position = skyplane_[index2].position;
			vertices[index].texture = skyplane_[index2].texture;
			indices[index] = index;
			index++;

			//左下
			vertices[index].position = skyplane_[index3].position;
			vertices[index].texture = skyplane_[index3].texture;
			indices[index] = index;
			index++;

			//左下
			vertices[index].position = skyplane_[index3].position;
			vertices[index].texture = skyplane_[index3].texture;
			indices[index] = index;
			index++;

			//右上
			vertices[index].position = skyplane_[index2].position;
			vertices[index].texture = skyplane_[index2].texture;
			indices[index] = index;
			index++;

			//右下
			vertices[index].position = skyplane_[index4].position;
			vertices[index].texture = skyplane_[index4].texture;
			indices[index] = index;
			index++;
		}
	}

	//頂点バッファの設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//ポインタを作成
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点バッファの作成に失敗");
		return false;
	}

	//インデックスバッファの設定
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//サブリソースへのポインタを作成
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファの作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("インデックスバッファの作成に失敗");
		return false;
	}

	//配列を解放
	indices.clear();
	vertices.clear();

	return true;
}

void Skyplane::render()
{
	//雲を移動
	transition_ += 0.0001F;
	if (transition_ > 1.0F)
	{
		transition_ -= 1.0F;
	}

	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	//頂点バッファをセット
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//インデックスバッファをセット
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//プリミティブタイプを設定
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Skyplane::destroy()
{
}

Matrix Skyplane::getWorldMatrix()
{
	world_ = Matrix::Identity;
	world_ *= Matrix::CreateScale(scale_);
	world_ *= Matrix::CreateRotationX(XMConvertToRadians(rotation_.x));
	world_ *= Matrix::CreateRotationY(XMConvertToRadians(rotation_.y));
	world_ *= Matrix::CreateRotationZ(XMConvertToRadians(rotation_.z));
	world_ *= Matrix::CreateTranslation(position_);

	return world_;
}

bool Skyplane::initskyPlane(const int PlaneResolution, const float PlaneWidth, const float PlaneTop, const float PlaneBottom, const int texRepeat)
{
	float quadsize;
	float radius;
	float constant;
	float texdelta;
	int index;
	Vector3 position;
	Vector2 texuv;

	//配列サイズを変更
	skyplane_.resize((PlaneResolution + 1) * (PlaneResolution + 1));

	//上空の各四角形のサイズを設定
	quadsize = PlaneWidth / static_cast<float>(PlaneResolution);

	//半径を計算
	radius = PlaneWidth / 2.0F;

	//高さの定数を計算
	constant = (PlaneTop - PlaneBottom) / (radius * radius);

	//テクスチャ座標を計算
	texdelta = static_cast<float>(texRepeat) / static_cast<float>(PlaneResolution);

	for (int i = 0; i <= PlaneResolution; i++)
	{
		position.z = (-0.5F * PlaneWidth) + (static_cast<float>(i) * quadsize);
		for (int j = 0; j <= PlaneResolution; j++)
		{
			//頂点座標を計算
			position.x = (-0.5F * PlaneWidth) + (static_cast<float>(j) * quadsize);
			position.y = PlaneTop - (constant * ((position.x * position.x) + (position.z * position.z)));

			//テクスチャ座標を計算
			texuv.x = static_cast<float>(j) - texdelta;
			texuv.y = static_cast<float>(i) - texdelta;

			//インデックスを計算
			index = i * (PlaneResolution + 1) + j;

			//座標を保存
			skyplane_[index].position = position;
			skyplane_[index].texture = texuv;
		}
	}

	return true;
}