#pragma once
//******************************
//		　　 マクロ
//******************************
#define _CRT_SECURE_NO_WARNINGS
#define DINPUT_VERTION 0x80 //DirectInputのバージョンマクロ
#define WIN32_LEAN_AND_MEAN //使われていないAPIの一部を除外するマクロ
#define STRICT				//型チェックを厳密にするマクロ

//******************************
//　　		include
//******************************
#include<Windows.h>
#include<d3d11.h>
#include<DirectXMath.h>
#include<tchar.h>
#include<SimpleMath.h>
#include<DDSTextureLoader.h>
#include<WICTextureLoader.h>
#include<math.h>
#include<dxgidebug.h>
#include<wrl/client.h>
#include<mmsystem.h>
#include<Pdh.h>
#include<dinput.h>
#include<d3dcompiler.h>
#include<DirectXColors.h>
#include<Shlwapi.h>

//std
#include<fstream>
#include<vector>
#include<map>
#include<chrono>
#include<iostream>

//******************************
//　　		リンク
//******************************
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"pdh.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"shlwapi.lib")

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
const bool kFullScreen = false;
const bool kvsync = true;
const float kScreen_depth = 100.0F;
const float kScreen_near = 1.0F;
const int kWindow_Height = 720;
const int kWindow_Width = 1280;
const int kExtensionTypeNum = 4;



//******************************
//　　		  列挙隊
//******************************



//******************************
//　　		  構造体
//******************************
