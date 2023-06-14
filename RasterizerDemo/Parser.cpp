#include "Parser.h"

Parser::Parser(std::string filePath)
{
	std::string line;
	std::string number = "";
	std::string materialNameToAdd = "";
	bool foundSameElement = false;
	int elementIndex = 0;

	std::vector<Vector3> vertexPositions;
	std::vector<Vector2> UVs;
	std::vector<Vector3> Normals;

	bool startedNumber = false;
	int n = 2;
	int nfloat = 0;
	float one = 0;
	float two = 0;
	float three = 0;
	int vertexPos = 0;
	int vertexUV = 0;
	int vertexNrml = 0;
	int indexStart = 0;
	int indexEnd = 0;
	int subMesh = 0;
	bool indexAdded = true;
	std::ifstream myFile(filePath);
	while (!myFile.eof())
	{
		n = 2;
		int verticesAdded = 0;
		getline(myFile, line);
		if (line.size() > 3)
		{
			if (line[0] == 'm' && line[1] == 't' && line[2] == 'l')
			{
				n = 7;
				while (n < line.size())
				{
					mtllib += line[n];
					n++;
				}
			}

			if (line[0] == 'u' && line[1] == 's' && line[2] == 'e')
			{
				n = 7;
				while (n < line.size())
				{
					materialNameToAdd += line[n];
					n++;
				}
				
				for (int i = 0; i < materialName.size(); i++)
				{
					if (materialName[i] == materialNameToAdd)
					{
						foundSameElement = true;
						elementIndex = i;
					}
				}

				if (foundSameElement == false)
				{
					materialName.push_back(materialNameToAdd);
					elementIndex = (int)materialName.size();
				}
				materialNameToAdd = "";
			}


			if (line[0] == 'o')
			{
				amountOfObjects++;
				if (indexAdded == false)
				{
					this->indices.push_back(Index(indexStart, indexEnd - 1));
					indexStart = indexEnd;
					indexEnd = indexStart;
					indexAdded = true;
				}
			}

			if (line[0] == 'v' && line[1] == ' ')
			{
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
						vertexPositions.push_back(Vector3(one, two, three));
					}
				}
			}
			else if (line[0] == 'v' && line[1] == 't')
			{
				while (n < line.size())
				{
					if (line[n] == ' ' && startedNumber == true)
					{
						if (nfloat == 0)
						{
							one = std::stof(number);
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
						two = std::stof(number);
						startedNumber = false;
						number = "";
						UVs.push_back(Vector2(one, two));
					}
				}
			}
			else if (line[0] == 'v' && line[1] == 'n')
			{
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
						Normals.push_back(Vector3(one, two, three));
					}
				}
			}
			else if (line[0] == 'f')
			{
				if (indexAdded == true)
				{
					indexAdded = false;
				}

				while (n < line.size())
				{
					if (line[n] == '/' && startedNumber == true)
					{
						if (vertexPos == 0)
						{
							vertexPos = std::stoi(number);
							number = "";
							startedNumber = false;
						}
						else if (vertexUV == 0)
						{
							vertexUV = std::stoi(number);
							number = "";
							startedNumber = false;
						}
					}
					else if (line[n] == ' ')
					{
						if (vertexNrml == 0 && vertexUV != 0)
						{
							vertexNrml = std::stoi(number);
							number = "";
							startedNumber = false;

							//Add vertex to buffer
							vertices.push_back(Vertex(vertexPositions[vertexPos - 1], Normals[vertexNrml - 1], UVs[vertexUV - 1], elementIndex - 1));
							verticesAdded++;
						}

						vertexPos = 0;
						vertexUV = 0;
						vertexNrml = 0;
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
						if (vertexNrml == 0)
						{
							vertexNrml = std::stoi(number);
							number = "";
							startedNumber = false;

							//Add vertex to buffer
							vertices.push_back(Vertex(vertexPositions[vertexPos - 1], Normals[vertexNrml - 1], UVs[vertexUV - 1], elementIndex - 1));
							verticesAdded++;
							
						}

						vertexPos = 0;
						vertexUV = 0;
						vertexNrml = 0;
					}
				}
				int startLocation = (int)vertices.size() - verticesAdded;
				if (verticesAdded == 3)
				{
					//Make a triangle
					testVertices.push_back(vertices[startLocation]);
					testVertices.push_back(vertices[startLocation + 1]);
					testVertices.push_back(vertices[startLocation + 2]);

					//Create index buffer list
					indexOrder.push_back(indexEnd);
					indexOrder.push_back(indexEnd + 1);
					indexOrder.push_back(indexEnd + 2);

					indexEnd += 3;
				}
				else if (verticesAdded == 4)
				{
					//Make a quad
					testVertices.push_back(vertices[startLocation]);
					testVertices.push_back(vertices[startLocation + 1]);
					testVertices.push_back(vertices[startLocation + 2]);

					testVertices.push_back(vertices[startLocation + 3]);
					testVertices.push_back(vertices[startLocation]);
					testVertices.push_back(vertices[startLocation + 2]);

					//Create index buffer list
					indexOrder.push_back(indexEnd);
					indexOrder.push_back(indexEnd + 1);
					indexOrder.push_back(indexEnd + 2);
					indexOrder.push_back(indexEnd + 3);
					indexOrder.push_back(indexEnd);
					indexOrder.push_back(indexEnd + 2);

					indexEnd += 6;
				}
			}
		}
	}

	mtlParser = new MtlParser(mtllib);

	for (int i = 0; i < testVertices.size(); i++)
	{
		int dex = testVertices[i].textureUsed;
		//testVertices[i].AddMtlValues(mtlParser->mtlinfo[dex].Ns, mtlParser->mtlinfo[dex].Ka, mtlParser->mtlinfo[dex].Kd, mtlParser->mtlinfo[dex].Ks, mtlParser->mtlinfo[dex].Ke, mtlParser->mtlinfo[dex].Ni, mtlParser->mtlinfo[dex].d);
	}
}

int Parser::sizeOfVertexBuffer() const
{
	return (int)(sizeof(Vertex) * testVertices.size());
}

int Parser::AmountOfVertices() const
{
	return (int)testVertices.size();
}

std::vector<Vertex> Parser::giveVertices() const
{
	return testVertices;
}

std::vector<Materialinfo> Parser::GiveMaterialInfo() const
{
	return mtlParser->GiveMaterial();
}

std::vector<D3D11_SUBRESOURCE_DATA> Parser::GiveData() const
{
	std::vector<D3D11_SUBRESOURCE_DATA> temp = mtlParser->datas;

	return temp;
}

void Parser::updateVerticesPos(float offSet)
{
	for (int i = 0; i < AmountOfVertices(); i++)
	{
		testVertices[i].OffSetPosition(offSet);
	}
}

