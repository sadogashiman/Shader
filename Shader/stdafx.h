#pragma once
//******************************
//		�@�@ �}�N��
//******************************
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN //�g���Ă��Ȃ�API�̈ꕔ�����O����}�N��
#define STRICT				//�^�`�F�b�N�������ɂ���}�N��
#define ALIGN16 _declspec(align(16)) //16�A���C�����g
//******************************
//�@�@		include
//******************************
#include<Windows.h>
#include<tchar.h>
#include<SimpleMath.h>
#include<math.h>
#include<wrl/client.h>
#include<mmsystem.h>
#include<Pdh.h>
#include<Shlwapi.h>

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
#include<list>
#include<random>
#include<algorithm>
#include<functional>

//******************************
//�@�@		�����N
//******************************
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"pdh.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"shlwapi.lib")

//******************************
//�@�@		���O���
//******************************
using namespace DirectX;
using namespace SimpleMath;
using namespace Microsoft::WRL;


//******************************
//�@�@		  �萔
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
const bool kTgs = false; //TGS�u�[�X�W���p�{�^���z�u

//******************************
//�@�@		  �񋓑�
//******************************



//******************************
//�@�@		  �\����
//******************************
