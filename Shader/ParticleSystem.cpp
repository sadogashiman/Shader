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

bool ParticleSystem::update(const float FrameTime)
{
	bool result;

	//古いパーティクルを破棄
	killParticle();

	//新しいパーティクルを放出
	emitParticle(FrameTime);

	//パーティクルを更新
	updateParticle(FrameTime);

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
	//バッファを破棄
	destroyBuffer();

	//パーティクルシステムを停止
	destroyParticleSystem();

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
	data_.max = 5000;

	//リストを作成
	particlelist_.resize(data_.max);

	std::list<std::unique_ptr<ParticleType>>::iterator itr;

	//リスト内のパーティクルの状態を初期化
	for (itr = particlelist_.begin(); itr != particlelist_.end(); itr++)
	{
		(*itr)->active = false;
	}

	//現在放出されているパーティクルの数を0にする
	currentcnt_ = 0;

	//1秒あたりのパーティクルの放出率の累積時間を初期化
	accumulatedtime_ = 0.0F;

	return true;
}

void ParticleSystem::destroyParticleSystem()
{
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
	for (int i = 0; i < indices.size(); i++)
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
	vertexdata.pSysMem = &vertices_;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, &vertexbuffer_);
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
	indexdata.pSysMem = &indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, &indexbuffer_);
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

	return true;
}

void ParticleSystem::renderBuffer()
{
}

void ParticleSystem::destroyBuffer()
{
}

void ParticleSystem::emitParticle(const float FrameTime)
{
	bool emit;
	bool found;
	Vector3 position;
	float velocity;
	Vector3 color;
	int index;
	std::random_device seed;
	std::mt19937 engine(seed());
	std::normal_distribution<> dist(1.0, 0.5);
	std::unique_ptr<ParticleType> newparticle;

	//フレーム時間を加算
	accumulatedtime_ += FrameTime;

	//フラグをオフに設定
	emit = false;

	//パーティクルを放出するタイミングかを判断
	if (accumulatedtime_ > (1000.0F / data_.perSecond))
	{
		accumulatedtime_ = 0.0F;
		emit = true;
	}

	//放出するパーティクルがある場合フレームごとに1つ放出
	if (emit && (accumulatedtime_ < (data_.max - 1)))
	{
		//総数を加算
		currentcnt_++;

		//ランダムにプロパティを設定し生成
		newparticle.get()->position.x = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.deviation.x;
		newparticle.get()->position.y = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.deviation.y;
		newparticle.get()->position.z = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.deviation.z;

		//速度を設定
		newparticle.get()->velocity = data_.velocity + (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.velocityVariation;

		//色を設定
		newparticle.get()->color.x = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;
		newparticle.get()->color.y = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;
		newparticle.get()->color.z = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;

		//設定したデータを末尾に追加
		particlelist_.push_back(std::move(newparticle));

		//Z軸を基準にlistをソート
		std::sort(particlelist_.begin()->get(), particlelist_.end()->get());
	}
}

void ParticleSystem::updateParticle(const float FrameTime)
{
	std::list<std::unique_ptr<ParticleType>>::iterator itr;

	//すべてのパーティクルを更新
	for (itr = particlelist_.begin(); itr != particlelist_.end(); itr++)
	{
		itr->get()->position.y -= itr->get()->velocity * FrameTime * 0.001F;
	}
}

void ParticleSystem::killParticle()
{
	std::list<std::unique_ptr<ParticleType>>::iterator itr;

	for (itr = particlelist_.begin(); itr != particlelist_.end(); itr++)
	{
		//一定の座標に行ったら要素を削除
		if (itr->get()->active && itr->get()->position.y < -3.0F)
		{
			particlelist_.erase(itr);
		}
	}

}
