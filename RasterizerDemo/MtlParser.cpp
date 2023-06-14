#include "MtlParser.h"
#include "stb_image.h"

MtlParser::MtlParser(std::string fileName)
{
	std::string line;
	std::string number = "";
	std::vector<float> floatValues;
	std::vector<Vector3> Kvalues;
	std::string materialName = "";
	std::string mapKd = "";
	bool startedNumber = false;
	int n = 0;
	int nfloat = 0;
	float one = 0;
	float two = 0;
	float three = 0;
	std::ifstream myFile("Models/" + fileName);
	while (!myFile.eof())
	{
		n = 0;
		getline(myFile, line);
		if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w')
		{
			n = 7;
			while (n < line.size())
			{
				materialName += line[n];
				n++;
			}
		}

		if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p')
		{
			n = 7;
			while (n < line.size())
			{
				mapKd += line[n];
				n++;
			}
		}

		if(line[0] == 'N' && line[1] == 's')
		{
			n = 3;
			while(n < line.size())
			{
				number += line[n];
				n++;
			}
			floatValues.push_back(stof(number));
			number = "";
		}

		if (line[0] == 'N' && line[1] == 'i')
		{
			n = 3;
			while (n < line.size())
			{
				number += line[n];
				n++;
			}
			floatValues.push_back(stof(number));
			number = "";
		}

		if (line[0] == 'd' && line[1] == ' ')
		{
			n = 2;
			while (n < line.size())
			{
				number += line[n];
				n++;
			}
			floatValues.push_back(stof(number));
			number = "";
		}

		if (line[0] == 'K' && line[1] == 'a')
		{
			n = 3;
			while (n < line.size())
			{
				if (line[n] == ' ' && startedNumber == true)
				{
					if (nfloat == 0)
					{
						one = std::stof(number);
						nfloat++;
					}
					else if (nfloat == 1)
					{
						two = std::stof(number);
					}

					startedNumber = false;
					number = "";
				}
				else
				{
					number += line[n];
					if (number.size() != 0)
					{
						startedNumber = true;
					}
				}

				n++;

				if (n == line.size())
				{
					three = std::stof(number);
					startedNumber = false;
					number = "";
					nfloat = 0;
					Kvalues.push_back(Vector3(one, two, three));
				}
			}
		}
		else if (line[0] == 'K' && line[1] == 'd')
		{
			n = 3;
			while (n < line.size())
			{
				if (line[n] == ' ' && startedNumber == true)
				{
					if (nfloat == 0)
					{
						one = std::stof(number);
						nfloat++;
					}
					else if (nfloat == 1)
					{
						two = std::stof(number);
					}

					startedNumber = false;
					number = "";
				}
				else
				{
					number += line[n];
					if (number.size() != 0)
					{
						startedNumber = true;
					}
				}

				n++;

				if (n == line.size())
				{
					three = std::stof(number);
					startedNumber = false;
					number = "";
					nfloat = 0;
					Kvalues.push_back(Vector3(one, two, three));
				}
			}
		}
		else if (line[0] == 'K' && line[1] == 's')
		{
			n = 3;
			while (n < line.size())
			{
				if (line[n] == ' ' && startedNumber == true)
				{
					if (nfloat == 0)
					{
						one = std::stof(number);
						nfloat++;
					}
					else if (nfloat == 1)
					{
						two = std::stof(number);
					}

					startedNumber = false;
					number = "";
				}
				else
				{
					number += line[n];
					if (number.size() != 0)
					{
						startedNumber = true;
					}
				}

				n++;

				if (n == line.size())
				{
					three = std::stof(number);
					startedNumber = false;
					number = "";
					nfloat = 0;
					Kvalues.push_back(Vector3(one, two, three));
				}
			}
		}
		else if (line[0] == 'K' && line[1] == 'e')
		{
			n = 3;
			while (n < line.size())
			{
				if (line[n] == ' ' && startedNumber == true)
				{
					if (nfloat == 0)
					{
						one = std::stof(number);
						nfloat++;
					}
					else if (nfloat == 1)
					{
						two = std::stof(number);
					}

					startedNumber = false;
					number = "";
				}
				else
				{
					number += line[n];
					if (number.size() != 0)
					{
						startedNumber = true;
					}
				}

				n++;

				if (n == line.size())
				{
					three = std::stof(number);
					startedNumber = false;
					number = "";
					nfloat = 0;
					Kvalues.push_back(Vector3(one, two, three));
				}
			}
		}

		if (mapKd != "")
		{
			mapKd = "Textures/" + mapKd;
			mtlinfo.push_back(Materialinfo(materialName, floatValues[0], Kvalues[0], Kvalues[1], Kvalues[2], Kvalues[3], floatValues[1], floatValues[2], mapKd));

			floatValues.clear();
			Kvalues.clear();
			materialName = "";
			mapKd = "";
		}
	}

	int width = 0;
	int height = 0;
	int components; //Define the int variables needed.

	if (mtlinfo.size() == 0)
	{
		mtlinfo.push_back(Materialinfo("Default", 500.0f, Vector3(0.8f, 0.8f, 0.8f), Vector3(0.8f, 0.8f, 0.8f), Vector3(0.8f, 0.8f, 0.8f), Vector3(0.8f, 0.8f, 0.8f), 0.0f, 1, "Textures/missingtextures.png"));
	}

	for (int i = 0; i < mtlinfo.size(); i++)
	{
		std::vector<unsigned char> textureDatatest;
		textureData.push_back(textureDatatest);
	}

	for (int i = 0; i < mtlinfo.size(); i++)
	{
		//Stbi_load opens up a file and gets the width of each row of pixels, the height of each column of pixels,
		//the components that make up each pixel (RGBA), specify the required amount of components you want from the image (For PNG 4 components are necessary otherwise an error will occur)
		unsigned char* throwawayTextureData = stbi_load(mtlinfo[i].MapKd.c_str(), &width, &height, &components, 4); //Put all of the pixel data into an unsigned char pointer.
		if (throwawayTextureData == NULL) //Something went wrong
		{
			throwawayTextureData = stbi_load("Textures/missingtextures.png", &width, &height, &components, 4); //Default
		}

		//std::vector<unsigned char> textureDatatest; //Define an unsigned char vector.
		int resize = width * height * 4; //Get the amount of pixels + each of their components
		textureData[i].resize(resize); //Set the appropriate size.

		//Copy over the data of throwawayTextureData to textureData so we can free the dynamically used memory.
		for (int h = 0; h < height; ++h)
		{
			for (int j = 0; j < width; ++j)
			{
				//Goes over every single pixel and transfers over their RGBA components to the vector.
				unsigned int pos = j * 4 + width * 4 * h;
				unsigned int pos0 = pos;
				unsigned int pos1 = pos + 1;
				unsigned int pos2 = pos + 2;
				unsigned int pos3 = pos + 3;
				textureData[i][pos0] = throwawayTextureData[pos0];
				textureData[i][pos1] = throwawayTextureData[pos1];
				textureData[i][pos2] = throwawayTextureData[pos2];
				textureData[i][pos3] = throwawayTextureData[pos3];
			}
		}

		D3D11_SUBRESOURCE_DATA data; //Specifies data for initializing a subresource
		data.pSysMem = &textureData[i][0]; //Pointer to intialization data.
		data.SysMemPitch = width * 4; //How long one entire row of pixels are in the image. (width * 4 cause each pixel has 4 elements)
		data.SysMemSlicePitch = height * width * 4; //Only used for 3D textures. It's the distance (in bytes) from the beginning of one depth level to the next.

		datas.push_back(data);

		stbi_image_free(throwawayTextureData); //Free the memory used by the image

		textureData[i].clear();
	}

	description.Width = width; //Width of 2D texture
	description.Height = height; //Height of 2D texture
	description.MipLevels = 1; //1 = Multisampled texture, 0 = full set of subtextures.
	description.ArraySize = (UINT)mtlinfo.size(); //Number of textures used in the array
	description.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32-bit unsigned-normalized-integer format that supports 8 bits per channel including alpha.
	description.SampleDesc.Count = 1; //Multisampling parameters. If count = 1 and quality = 0, then it's the default sampler mode with no anti-aliasing.
	description.SampleDesc.Quality = 0;
	description.Usage = D3D11_USAGE_IMMUTABLE; //Can only be read by the GPU, not written, and cannot be accessed at all by the CPU.
	description.BindFlags = D3D11_BIND_SHADER_RESOURCE; //Bind a buffer or texture to a shader stage.
	description.CPUAccessFlags = 0; //CPU access is not required (Usage also makes it inaccessible by the CPU)
	description.MiscFlags = 0; //No misc flags apply.
}

std::vector<Materialinfo> MtlParser::GiveMaterial() const
{
	return mtlinfo;
}

std::vector<D3D11_SUBRESOURCE_DATA> MtlParser::GiveDatas() const
{
	return datas;
}
