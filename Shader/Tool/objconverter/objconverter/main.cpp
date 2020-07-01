#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include<iostream>
#include<Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
struct VertexType
{
	float x, y, z;
};

struct FaceType
{
	int vindex1, vindex2, vindex3;
	int tindex1, tindex2, tindex3;
	int nindex1, nindex2, nindex3;
};

void getModelFileName(char* FileName);
bool readFileCount(char* FileName, int& VertexCount, int& TextureCount, int& NormalCount, int& FaceCount);
bool loadDataStructures(char* FileName, int VertexCount, int TextureCount, int NormalCount, int FaceCount);

int main()
{
	bool result;
	char filename[256];
	int vertexcnt = 0;;
	int texturecnt = 0;;
	int normalcnt = 0;
	int facecnt = 0;
	int garbage;

	do
	{
		//モデルファイル名を取得
		getModelFileName(filename);

		result = readFileCount(filename, vertexcnt, texturecnt, normalcnt, facecnt);
		if (!result)
		{
			return false;
		}

		std::cout << std::endl;
		std::cout << "頂点数 : " << vertexcnt << std::endl;
		std::cout << "UV : " << texturecnt << std::endl;
		std::cout << "法線 : " << normalcnt << std::endl;
		std::cout << "フェース : " << facecnt << std::endl;

		result = loadDataStructures(filename, vertexcnt, texturecnt, normalcnt, facecnt);
		if (!result)
		{
			return -1;
		}

		std::cout << "変換が正常に終了しました" << std::endl;
		std::cout << "変換を続けますか? (Yes…1/No…0)" << std::endl;
		std::cin >> garbage;

	} while (garbage != 0);

	return 0;
}

void getModelFileName(char* FileName)
{
	bool done = false;
	std::ifstream fin;
	while (!done)
	{
		std::cout << "ファイル名を入力してください : ";
		
		std::cin >> FileName;

		fin.open(FileName);

		if (fin.good())
		{
			done = true;
		}
		else
		{
			fin.clear();
			std::cout << std::endl;
			std::cout << "ファイル " << FileName << " が展開できません" << std::endl << std::endl;

		}
	}

}

bool readFileCount(char* FileName, int& VertexCount, int& TextureCount, int& NormalCount, int& FaceCount)
{
	std::ifstream fin;
	char input;
	VertexCount = 0;
	TextureCount = 0;
	NormalCount = 0;
	FaceCount = 0;

	fin.open(FileName);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);

	//要素数をカウント
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
		
			if (input == ' ')
				VertexCount++;
			else if (input == 't')
				TextureCount++;
			else if (input == 'n')
				NormalCount++;
		}
		else if (input == 'f')
		{
			fin.get(input);

			if (input == ' ')
				FaceCount++;
		}
		else if (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	return true;
}

bool loadDataStructures(char* FileName, int VertexCount, int TextureCount, int NormalCount, int FaceCount)
{
	VertexType* vertices;
	VertexType* texcoords;
	VertexType* normals;
	FaceType* faces;
	int vertexindex;
	int texcoordindex;
	int normalindex;
	int faceindex;
	int vindex;
	int tindex;
	int nindex;
	char input;
	char input2;
	std::ofstream fout;
	std::ifstream fin;

	vertices = new VertexType[VertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[TextureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[NormalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[FaceCount];
	if (!faces)
	{
		return false;
	}

	//インデックスを初期化
	vertexindex = 0;
	texcoordindex = 0;
	normalindex = 0;
	faceindex = 0;

	//ファイル展開
	fin.open(FileName);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);

	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			if (input==' ')
			{
				fin >> vertices[vertexindex].x >> vertices[vertexindex].y >> vertices[vertexindex].z;
				vertices[vertexindex].z = vertices[vertexindex].z * -1.0F; //Z軸反転
				vertexindex++;
			}
			else if (input == 't')
			{
				fin >> texcoords[texcoordindex].x >> texcoords[texcoordindex].y;
				texcoords[texcoordindex].y = 1.0F - texcoords[texcoordindex].y; //左手座標系に変換
				texcoordindex++;
			}
			else if (input == 'n')
			{
				fin >> normals[normalindex].x >> normals[normalindex].y >> normals[normalindex].z;
				normals[normalindex].z = normals[normalindex].z * -1.0F; //Z軸反転
				normalindex++;
			}
		}
		else if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[faceindex].vindex3 >> input2 >> faces[faceindex].tindex3 >> input2 >> faces[faceindex].nindex3
					>> faces[faceindex].vindex2 >> input2 >> faces[faceindex].tindex2 >> input2 >> faces[faceindex].nindex2
					>> faces[faceindex].vindex1 >> input2 >> faces[faceindex].tindex1 >> input2 >> faces[faceindex].nindex1;
				faceindex++;
			}
		}

		while (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	char output[MAX_PATH];
	wchar_t tmp[MAX_PATH];
	mbstowcs(tmp, FileName, MAX_PATH);
	
	//拡張子をtxtに変換してファイル名指定
	PathRenameExtension(tmp, L".txt");

	wcstombs(output, tmp, MAX_PATH);

	//出力量ファイル展開
	fout.open(output);

	fout << "VertexCount : " << (FaceCount * 3) << std::endl;
	fout << std::endl;
	fout << "Data : " << std::endl;

	//データ出力
	for (int i = 0; i < faceindex; i++)
	{
		vindex = faces[i].vindex1 - 1;
		tindex = faces[i].tindex1 - 1;
		nindex = faces[i].nindex1 - 1;

		fout << vertices[vindex].x << ' ' << vertices[vindex].y << ' ' << vertices[vindex].z << ' '
			<< texcoords[tindex].x << ' ' << texcoords[tindex].y << ' '
			<< normals[nindex].x << ' ' << normals[nindex].y << ' ' << normals[nindex].z << std::endl;

		vindex = faces[i].vindex2 - 1;
		tindex = faces[i].tindex2 - 1;
		nindex = faces[i].nindex2 - 1;

		fout << vertices[vindex].x << ' ' << vertices[vindex].y << ' ' << vertices[vindex].z << ' '
			<< texcoords[tindex].x << ' ' << texcoords[tindex].y << ' '
			<< normals[nindex].x << ' ' << normals[nindex].y << ' ' << normals[nindex].z << std::endl;

		vindex = faces[i].vindex3 - 1;
		tindex = faces[i].tindex3 - 1;
		nindex = faces[i].nindex3 - 1;

		fout << vertices[vindex].x << ' ' << vertices[vindex].y << ' ' << vertices[vindex].z << ' '
			<< texcoords[tindex].x << ' ' << texcoords[tindex].y << ' '
			<< normals[nindex].x << ' ' << normals[nindex].y << ' ' << normals[nindex].z << std::endl;
	}

	fout.close();

	//データ解放
	delete[] vertices;
	delete[] texcoords;
	delete[] normals;
	delete[] faces;


	return true;
}
