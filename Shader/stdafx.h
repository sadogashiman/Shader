#pragma once
//******************************
//		　　 マクロ
//******************************
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN //使われていないAPIの一部を除外するマクロ
//******************************
//　　		include
//******************************
#include<Windows.h>
#include<tchar.h>
#include<SimpleMath.h>
#include<math.h>
#include<wrl/client.h>
#include<mmsystem.h>
#include<Shlwapi.h>
#include<cassert>

//Lua関係
#include <lua.hpp>
#include <lualib.h>
#include <lauxlib.h>

//DirectX
#include<DirectXMath.h>
#include<DirectXColors.h>
#include<d3dcompiler.h>
#include<dinput.h>
#include<dxgidebug.h>
#include<d3d11.h>
#include<DDSTextureLoader.h>
#include<WICTextureLoader.h>

//std
#include<fstream>
#include<vector>
#include<map>
#include<chrono>
#include<iostream>
#include<random>
#include<algorithm>
#include<thread>
#include<mutex>
#include<exception>
#include<stdexcept>

//******************************
//　　		リンク
//******************************
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"lua.lib")

//******************************
//　　		名前空間
//******************************
using namespace DirectX;
using namespace SimpleMath;
using namespace Microsoft::WRL;


//******************************
//　　		  定数
//******************************
const int kScreenWidth = 1280;
const int kScreenHeight = 720;
const int kWindow_Width = 1280;
const int kWindow_Height = 720;
const int kExtensionTypeNum = 5;
const float kScreen_depth = 1000.0F;
const float kScreen_near = 0.1F;
const bool kFullScreen = false;
const bool kvsync = true;
const bool kTgs = false; //TGSブース展示用ボタン配置

//******************************
//　　		  列挙体
//******************************



//******************************
//　　		  構造体
//******************************
