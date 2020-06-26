#include "ParticleSystem.h"
#include"Support.h"
#include"Direct3D.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::init(const wchar_t* FileName)
{
	bool result;

	//ファイル名をローカルに保存
	wcscpy(filename_, FileName);

	//パーティクルシステムの初期化
	result = initParticleSystem();
	if (!result)
	{
		Error::showDialog("パーティクルシステムの初期化に失敗");
		return false;
	}

	//バッファの初期化
	result = initbuffer();
	if (!result)
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}

	return true;
}

bool ParticleSystem::update()
{
	bool result;

	//古いパーティクルを破棄
	killParticle();

	//新しいパーティクルを放出
	emitParticle();

	//パーティクルを更新
	updateParticle();

	//動的頂点バッファを更新
	result = updateBuffer();
	if (!result)
	{
		Error::showDialog("バッファの更新に失敗");
		return false;
	}

	return true;
}

void ParticleSystem::render()
{
	//グラフィックスパイプラインに配置
	renderBuffer();
}

void ParticleSystem::destroy()
{
	//テクスチャを解放
	TextureFactory::getInstance()->deleteTexture(filename_);
}

bool ParticleSystem::initParticleSystem()
{

	//放出位置を設定
	data_.deviation.x = 0.5F;
	data_.deviation.y = 0.1F;
	data_.deviation.z = 2.0F;

	//加速度を設定
	data_.velocity = 1.0F;
	data_.velocityVariation = 0.2F;

	//サイズを設定
	data_.size = 0.2F;

	//1秒あたりの放出数を設定
	data_.perSecond = 250.0F;

	//パーティクルの最大数を設定
	data_.max = 2;

	//リストを作成
	particlevector_.resize(data_.max);

	for (auto& itr : particlevector_)
	{
		itr.active = false;
	}

	//現在放出されているパーティクルの数を0にする
	currentcnt_ = 0;

	//1秒あたりのパーティクルの放出率の累積時間を初期化
	accumulatedtime_ = 0.0F;

	return true;
}

bool ParticleSystem::initbuffer()
{
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//頂点配列の最大数を設定
	vertexcnt_ = data_.max * 6;

	//インデックス配列の最大数を設定
	indexcnt_ = vertexcnt_;

	//レンダリングされるパーティクルの配列を作成
	vertices_.resize(vertexcnt_);
	if (vertices_.empty())
	{
		Error::showDialog("頂点配列のサイズが0です");
		return false;
	}

	//インデックス配列を作成
	indices.resize(indexcnt_);
	if (indices.empty())
	{
		Error::showDialog("インデックス配列のサイズが0です");
		return false;
	}

	//配列を初期化
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		indices[i] = i;
	}

	//動的頂点バッファの設定
	vertexbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//サブリソースへのポインタを作成
	vertexdata.pSysMem = &vertices_[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//静的インデックスバッファの設定
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcnt_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//サブリソースへのポインタを作成
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//不要になったので内部情報を破棄
	indices.clear();

	return true;
}

bool ParticleSystem::updateBuffer()
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	VertexType* verticesptr;
	int index;

	ZeroMemory(&vertices_[0], sizeof(VertexType)*vertexcnt_);

	//インデックスを初期化
	index = 0;
	for (int i = 0;i<currentcnt_;i++)
	{
		//左下
		vertices_[index].position = Vector3(particlevector_[i].position.x - data_.size, particlevector_[i].position.y - data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(0.0F, 1.0F);
		vertices_[index].color = Vector4(particlevector_[i].color.x, particlevector_[i].color.y, particlevector_[i].color.z, 1.0F);
		index++;

		//左上
		vertices_[index].position = Vector3(particlevector_[i].position.x - data_.size, particlevector_[i].position.y + data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(0.0F, 0.0F);
		vertices_[index].color = Vector4(particlevector_[i].color.x, particlevector_[i].color.y, particlevector_[i].color.z, 1.0F);
		index++;

		//右下
		vertices_[index].position = Vector3(particlevector_[i].position.x + data_.size, particlevector_[i].position.y - data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(1.0F, 1.0F);
		vertices_[index].color = Vector4(particlevector_[i].color.x, particlevector_[i].color.y, particlevector_[i].color.z, 1.0F);
		index++;

		//右下
		vertices_[index].position = Vector3(particlevector_[i].position.x + data_.size, particlevector_[i].position.y - data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(1.0F, 1.0F);
		vertices_[index].color = Vector4(particlevector_[i].color.x, particlevector_[i].color.y, particlevector_[i].color.z, 1.0F);
		index++;

		//左上
		vertices_[index].position = Vector3(particlevector_[i].position.x - data_.size, particlevector_[i].position.y + data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(0.0F, 0.0F);
		vertices_[index].color = Vector4(particlevector_[i].color.x, particlevector_[i].color.y, particlevector_[i].color.z, 1.0F);
		index++;

		//右上
		vertices_[index].position = Vector3(particlevector_[i].position.x + data_.size, particlevector_[i].position.y + data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(1.0F, 0.0F);
		vertices_[index].color = Vector4(particlevector_[i].color.x, particlevector_[i].color.y, particlevector_[i].color.z, 1.0F);
		index++;

	}

	//頂点バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(vertexbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("頂点バッファのロックに失敗");
		return false;
	}

	//シェーダーへのポインタを作成
	verticesptr = (VertexType*)mappedresource.pData;

	//頂点バッファをコピー
	memcpy(verticesptr, (void*)vertices_.data(), sizeof(VertexType) * vertexcnt_);

	//頂点バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(vertexbuffer_.Get(), 0);

	return true;
}

void ParticleSystem::renderBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//幅とオフセットを設定
	stride = sizeof(VertexType);
	offset = 0;

	//頂点バッファを入力アセンブラでアクティブに設定
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//インデックスバッファを入力アセンブラでアクティブに設定
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//頂点のプリミティブタイプを設定
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleSystem::emitParticle()
{
	bool emit;
	Vector3 position;
	Vector3 color;
	std::random_device seed;
	std::mt19937 engine(seed());
	std::normal_distribution<> dist(1.0, 0.5);
	ParticleType newparticle;

	//フレーム時間を加算
	accumulatedtime_ += kFrameTime;

	//フラグをオフに設定
	emit = false;

	//パーティクルを放出するタイミングかを判断
	if (accumulatedtime_ > (1000.0F / data_.perSecond))
	{
		accumulatedtime_ = 0.0F;
		emit = true;
	}

	//放出するパーティクルがある場合フレームごとに1つ放出
	if (emit && (currentcnt_ < (data_.max - 1)))
	{
		//ランダムにプロパティを設定し生成
		newparticle.position.x = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) * data_.deviation.x;
		newparticle.position.y = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) * data_.deviation.y;
		newparticle.position.z = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) * data_.deviation.z;

		//速度を設定
		newparticle.velocity = data_.velocity + (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) * data_.velocityVariation;

		//色を設定
		newparticle.color.x = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;
		newparticle.color.y = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;
		newparticle.color.z = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;

		//設定したデータを末尾に追加
		particlevector_[currentcnt_++] = newparticle;

		//Z軸を基準にソート
		std::sort(particlevector_.begin(), particlevector_.end(), [](const ParticleType& First, const ParticleType& Second) {return First.position.z < Second.position.z; });
	}
}

void ParticleSystem::updateParticle()
{
	for (auto& itr : particlevector_)
	{
		itr.position.y -= itr.velocity * kFrameTime * 0.001F;
	}

}

void ParticleSystem::killParticle()
{
	for (auto itr = particlevector_.begin(); itr != particlevector_.end();)
	{
		if (itr->active && itr->position.y < -3.0F)
		{
			itr->active = false;
			currentcnt_--;
			itr = particlevector_.erase(itr);
		}
		else
			++itr;
	}


	particlevector_.resize(data_.max);

}
