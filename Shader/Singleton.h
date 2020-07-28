#pragma once
#include<cassert>
#include<mutex>
class SingletonFinalizer
{
public:
	using finalizerFunc = void(*)();
	static void addFinalizer(finalizerFunc Func);
	static void finalize();
};

template<typename T>
class Singleton final
{
private:
	static T* instance_;
	static std::once_flag initflag_;

	//インスタンス作成
	inline static void create()
	{
		instance_ = new T;
		SingletonFinalizer::addFinalizer(&Singleton<T>::destroy());
	}

	//破棄
	inline static void destroy()
	{
		delete instance_;
		instance_ = nullptr;
	}

public:
	//インスタンス取得
	inline static T& getInstance() 
	{
		std::call_once(initflag_, create);
		return *instance_;
	}
};

template <typename T> std::once_flag Singleton<T>::initflag_;
template <typename T> T* Singleton<T>::instance_ = nullptr;