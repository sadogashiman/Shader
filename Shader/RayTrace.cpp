#include "stdafx.h"
#include "RayTrace.h"
#include "System.h"

RayTrace::RayTrace()
{
	ZeroMemory(this, sizeof(this));
	camerapos_ = Vector3(0.0F, 0.0F, -0.5F);
	lightpos_ = Vector3(-10.0F, 10.0F, -5.0F);
}

RayTrace::~RayTrace()
{
}

void RayTrace::raySphere(Vector3& CameraPosition, Vector3& EyeDirection, Sphere& SphereArray, Vector3 Intersection, Vector3 NormalAtIntersection, float* RayLenght)
{
	float t = 0; //例の交差点までの長さ
	float a,b,c; //2次式の係数
	float d = 0; //判別式


	XMStoreFloat(&a,XMVector3Dot(EyeDirection, EyeDirection));
	float tmp;
	XMStoreFloat(&tmp, XMVector3Dot(CameraPosition, EyeDirection));
	b = 2.0F * tmp;
	XMStoreFloat(&tmp, XMVector3Dot(CameraPosition, CameraPosition));
	c = tmp -  SphereArray.rdius * SphereArray.rdius;

	d = b * b - 4 * a * c;

	if (d >= 0)
	{
		//交点が一つ又は二つの時
		t = (-b - sqrt(d)) / 2 * a;
	}
	else
	{
		//交点なし
		t = -1;
	}

	Intersection = CameraPosition + t * EyeDirection; //レイの式をそのまま使用
	NormalAtIntersection = Intersection - SphereArray.position;//法線は級の中心から交点へ向かうベクトル
	NormalAtIntersection = XMVector3Normalize(NormalAtIntersection);
	*RayLenght = t;
}

bool RayTrace::render()
{
	Vector3 finalcolor;
	finalcolor = Vector3::Zero;
	Vector3 spherecolor;
	spherecolor = Vector3::Zero;

	InvalidateRect(System::getWindowHandle(), NULL, false);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(System::getWindowHandle(), &ps);
	//スクリーン平面x,y
	int sx = 0;
	int sy = 0;
	Vector3 pixelpos; //当該ピクセルの3D空間上の位置
	pixelpos = Vector3::Zero;
	Vector3 eyedir; //視線方向
	eyedir = Vector3::Zero;

	//画素数分だけループ
	for (sy = 0; sy < kWindow_Height; sy++)
	{
		for (sx = 0; sx < System::getWindowWidth(); sx++)
		{
			finalcolor = Vector3(80.0F, 80.0F, 80.0F);
			spherecolor = Vector3::Zero;
			
			//レイとの交差判定
			pixelpos = transScreenToWorld(sx, sy);

			float dist = 0.0F;
			Vector3 cross; //交点
			Vector3 normal;//球の法線
			eyedir = pixelpos - camerapos_;
			eyedir = XMVector3Normalize(eyedir);
			raySphere(camerapos_, eyedir, *spherearray_[0], cross, normal, &dist);

			//交差判定があるか確認
			if (dist > 0)
			{
				Vector3 light = lightpos_ - cross;
				light = XMVector3Normalize(light);
				spherecolor = lambertShade(light, normal, Vector3(3.0F, 125.0F, 251.0F));
				finalcolor = spherecolor;
			}

			DWORD col = RGB(finalcolor.x, finalcolor.y, finalcolor.z);
			SetPixel(hdc, sx, sy, col);
		}
	}

	EndPaint(System::getWindowHandle(), &ps);

	return true;
}

void RayTrace::addSphere(Sphere* sp)
{
	spherearray_[sphere_num_] = new Sphere;
	memcpy(spherearray_[sphere_num_], sp, sizeof(Sphere));
	sphere_num_++;
}

Vector3 RayTrace::transScreenToWorld(const int X, const int Y)
{
	Vector3 screenpos;

	screenpos.z = 0;
	screenpos.x = (static_cast<float>(X) / static_cast<float>(kScreenWidth)) * 2 - 1.0F;
	screenpos.y = (static_cast<float>(Y) / static_cast<float>(kScreenHeight)) * 2 - 1.0F;

	return screenpos;
}

Vector3 RayTrace::lambertShade(Vector3 PixelColor, Vector3 NormalAtIntersection,Vector3 DiffuseColor)
{
	Vector3 color(0, 0, 0);
	float ln;

	XMStoreFloat(&ln, XMVector3Dot(PixelColor, NormalAtIntersection));
	ln = max(0, ln);
	color = Vector3(DiffuseColor.x * ln , DiffuseColor.y * ln, DiffuseColor.z * ln);

	return color;

}
