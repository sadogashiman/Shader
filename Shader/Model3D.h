#pragma once
#include"Direct3D.h"

class Model3D
{
protected:
	Direct3D* instanceptr_; //Direct3Dクラスのインスタンスポインタ
public:
	Model3D() { instanceptr_ = Direct3D::getInstance(); }
	virtual ~Model3D() = default;
	virtual bool init(const wchar_t* ModelFileName,const wchar_t* TextureFileName)=0;
	virtual void render() = 0;
	virtual void destroy() = 0;

	//get
	virtual const int getIndexCount()const = 0;
	virtual Matrix getWorldMatrix()=0;
};

