#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "Vector3.h"
#include <d3d11.h>

struct Materialinfo
{
	std::string MaterialName;
	float Ns;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ke[3];
	float Ni;
	float d;
	std::string MapKd;

	Materialinfo(std::string name, float NsVal, Vector3 KaVal, Vector3 KdVal, Vector3 KsVal, Vector3 KeVal, float NiVal, float dVal, std::string MapName)
	{
		MaterialName = name;
		Ns = NsVal;
		Ka[0] = KaVal.GetX();
		Ka[1] = KaVal.GetY();
		Ka[2] = KaVal.GetZ();
		Kd[0] = KdVal.GetX();
		Kd[1] = KdVal.GetY();
		Kd[2] = KdVal.GetZ();
		Ks[0] = KsVal.GetX();
		Ks[1] = KsVal.GetY();
		Ks[2] = KsVal.GetZ();
		Ke[0] = KeVal.GetX();
		Ke[1] = KeVal.GetY();
		Ke[2] = KeVal.GetZ();
		Ni = NiVal;
		d = dVal;
		MapKd = MapName;
	}
};

class MtlParser
{
private:

public:

	MtlParser(std::string fileName);

	std::vector<std::vector<unsigned char>> textureData;
	std::vector<Materialinfo> mtlinfo;
	D3D11_TEXTURE2D_DESC description;
	std::vector<D3D11_SUBRESOURCE_DATA> datas;

	std::vector<Materialinfo> GiveMaterial() const;
	std::vector<D3D11_SUBRESOURCE_DATA> GiveDatas() const;
};

