#pragma once
class Model3D
{
protected:
public:
	virtual ~Model3D() = default;
	virtual bool init(const wchar_t* ModelFileName,const wchar_t* TextureFileName)=0;
	virtual void render() = 0;
	virtual void destroy() = 0;

	//get
	virtual const int getIndexCount()const = 0;
	virtual Matrix getWorldMatrix()=0;
};

