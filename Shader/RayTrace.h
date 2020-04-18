#pragma once
struct Sphere
{
	Vector3 position;
	float rdius;
};
class RayTrace
{
private:

	unsigned long sphere_num_;
	unsigned long box_num_;
	Vector3 camerapos_;
	Vector3 lightpos_;
	Sphere* spherearray_[100];

protected:

public:
	RayTrace();
	~RayTrace();
	void raySphere(Vector3& CameraPosition, Vector3& EyeDirection, Sphere& SphereArray, Vector3 Intersection, Vector3 NormalAtIntersection,float* RayLenght); //ƒŒƒC‚Æ‹…‚ÌŒğ·”»’èŠÖ”
	bool render();
	void addSphere(Sphere* Sphere);
	Vector3 transScreenToWorld(const int X, const int Y);
	Vector3 lambertShade(Vector3 PixelColor, Vector3 NormalAtIntersection, Vector3 DiffuseColor);


};

