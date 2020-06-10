#pragma once
class Model3D
{
protected:

public:
	virtual bool init(const wchar_t* ModelFileName)=0;
	virtual void render() = 0;
	virtual void destroy() = 0;
	virtual const int getIndexCount()const = 0;
};
