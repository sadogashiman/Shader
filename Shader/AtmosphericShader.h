#pragma once
class AtmosphericShader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	bool setShaderParameters(Matrix World,Matrix View,Matrix Projection);
	void renderShader(const int IndexCount);
public:
	bool init();
	bool render();
};

