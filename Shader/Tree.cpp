#include "stdafx.h"
#include "Tree.h"
#include "Direct3D.h"
Tree::Tree()
{
}

Tree::~Tree()
{
}

bool Tree::init(const wchar_t* TrunkModelFileName, const wchar_t* LeafModelFileName, float Scale)
{
	bool result;

	//サポートクラスを生成
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//モデルの読み込み
	result = loadModel(trunkfilename_);
	if (!result)
	{
		Error::showDialog("幹のモデルの読み込みに失敗");
		return false;
	}

	//インデックスカウントをコピー
	trunkindexcount_ = indexcount_;

	//木の幹のバッファを初期化
	result = initTrunkBuffer();
	if (!result)
	{
		Error::showDialog("木の幹のバッファの初期化に失敗");
		return false;
	}

	//モデルがバッファに読み込まれたので解放
	releaseModel();

	//葉っぱのモデルを読み込み
	result = initLeafBuffer();
	if (!result)
	{
		Error::showDialog("葉っぱモデルの読み込みに失敗");
		return false;
	}

	//インデックスカウントをコピー
	leafindexcount_ = indexcount_;

	//葉っぱのバッファを初期化
	result = initLeafBuffer();
	if (!result)
	{
		Error::showDialog("葉っぱのバッファの初期化に失敗");
		return false;
	}

	//バッファに読み込まれたのでモデルを解放
	releaseModel();

	//モデルファイル名から拡張子違いの画像ファイル名を生成
	wcscpy(trunkfilename_, support_.get()->renameToJPEG(TrunkModelFileName));
	wcscpy(leaffilename_, support_.get()->renameToJPEG(LeafModelFileName));

	return true;
}

void Tree::renderTrunk()
{
	//幹をレンダリング
	renderTrunkBuffer();
}

void Tree::renderLeaves()
{
	//葉をレンダリング
	renderLeavesBuffer();
}

void Tree::destroy()
{
	//このクラス内で使用した各データの破棄
	destroyBuffer();
	releaseTexture();
	releaseModel();
}

bool Tree::initTrunkBuffer()
{
	std::vector<VertexType> vertices;
	std::vector<unsigned long>indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//頂点配列を作成
	vertices.resize(vertexcount_);

	//インデックス配列を作成
	indices.resize(indexcount_);

	//頂点配列とインデックス配列にデータをロード
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = Vector3(model_[i].position.x * scale_, model_[i].position.y * scale_, model_[i].position.z * scale_);
		vertices[i].texture = model_[i].texpos;
		vertices[i].normal = model_[i].normalpos;

		indices[i] = i;
	}

	//頂点バッファの設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType)*vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;
	
	//ポインタを作成
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, trunkvertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点バッファの作成に失敗");
		return false;
	}

	//インデックスバッファの設定
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth= sizeof(unsigned long) * indexcount_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//インデックスバッファのポインタを作成
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファの作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, trunkindexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("インデックスバッファの作成に失敗");
		return false;
	}

	//不要なデータの削除
	vertices.clear();
	indices.clear();

	return true;
}

bool Tree::initLeafBuffer()
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//頂点配列を作成
	vertices = new VertexType[vertexcount_];
	if (!vertices)
	{
		Error::showDialog("頂点配列のメモリ確保に失敗");
		return false;
	}

	//インデックス配列を作成
	indices = new unsigned long[indexcount_];
	if (!indices)
	{
		Error::showDialog("インデックス配列のメモリ確保に失敗");
		return false;
	}

	//頂点配列とインデックス配列にデータをロード
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = Vector3(model_[i].position.x * scale_, model_[i].position.y * scale_, model_[i].position.z * scale_);
		vertices[i].texture = model_[i].texpos;
		vertices[i].normal = model_[i].normalpos;

		indices[i] = i;
	}

	//頂点バッファの設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//ポインタを作成
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, leafvertexbuffer_.GetAddressOf());
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

	//インデックスバッファのポインタを作成
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファの作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, leafindexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("インデックスバッファの作成に失敗");
		return false;
	}

	//不要なデータの削除
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;
	return true;
}

void Tree::renderTrunkBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//幅とオフセットを設定
	stride = sizeof(VertexType);
	offset = 0;

	//頂点バッファをレンダリングできるようにセット
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, trunkindexbuffer_.GetAddressOf(), &stride, &offset);

	//インデックスバッファをレンダリングできるようにセット
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(trunkindexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//レンダリングのタイプを設定	
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Tree::renderLeavesBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//幅とオフセットを設定
	stride = sizeof(VertexType);
	offset = 0;

	//頂点バッファをレンダリングできるようにセット
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, leafindexbuffer_.GetAddressOf(), &stride, &offset);

	//インデックスバッファをレンダリングできるようにセット
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(leafindexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//レンダリングのタイプを設定	
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool Tree::loadTexture()
{

	return true;
}

void Tree::releaseTexture()
{
	//このクラスで使用した画像ファイルを破棄
	TextureFactory::getInstance()->deleteTexture(trunkfilename_);
	TextureFactory::getInstance()->deleteTexture(leaffilename_);
}

bool Tree::loadModel(const wchar_t* ModelName)
{
	std::fstream fp;
	char input;

	//ファイルパスが有効か確認
	if (!Support::searchFile(ModelName))
	{
		return false;
	}

	//ファイルパスが有効なのを確認したので展開
	fp.open(ModelName, std::ios::beg | std::ios::in);

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
	model_.resize(vertexcount_);

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
		fp >> model_[i].position.x >> model_[i].position.y >> model_[i].position.z;
		fp >> model_[i].texpos.x >> model_[i].texpos.y;
		fp >> model_[i].normalpos.x >> model_[i].normalpos.y >> model_[i].normalpos.z;
	}

	fp.close();
	return true;
}

void Tree::releaseModel()
{
	//モデルデータを破棄
	model_.clear();
}

void Tree::destroyBuffer()
{
}
