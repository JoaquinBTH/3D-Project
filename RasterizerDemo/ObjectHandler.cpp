#include "ObjectHandler.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool ObjectHandler::LoadMaterial(ID3D11Device* device, std::string fileName)
{
    //Open the new file
    fileName = "Models/" + fileName;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return false; //File not found
    }

    //Create the necessary variables
    std::string line;
    std::string materialName;
    float specularExponent = 0.0f;
    std::string diffuseMap = "";
    std::string specularMap = "";
    std::string ambientMap = "";
    std::vector<std::string> diffuseMaps;
    std::vector<std::string> ambientMaps;
    std::vector<std::string> specularMaps;
    bool exists = false;
    int currentMaterialIndex = 0;

    //Parse each line in the MTL file
    while (std::getline(file, line))
    {
        std::stringstream ss(line); //Load line into a stringstream

        //Get the prefix
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl")
        {
            //Check if material name exists
            exists = false;
            ss >> materialName;
            for (int i = 0; i < materials.size(); i++)
            {
                if (materialName == materials[i].name)
                {
                    exists = true;
                    currentMaterialIndex = i;
                    diffuseMaps.push_back("");
                    ambientMaps.push_back("");
                    specularMaps.push_back("");
                    break;
                }
            }
        }

        if (exists)
        {
            if (prefix == "Ns")
            {
                ss >> specularExponent;
                materials[currentMaterialIndex].specularExponent = specularExponent;
            }
            else if (prefix == "map_Kd")
            {
                ss >> diffuseMap;
                diffuseMaps.back() = "Textures/" + diffuseMap;
            }
            else if (prefix == "map_Ka")
            {
                ss >> ambientMap;
                ambientMaps.back() = "Textures/" + ambientMap;
            }
            else if (prefix == "map_Ks")
            {
                ss >> specularMap;
                specularMaps.back() = "Textures/" + specularMap;
            }
        }
    }

    if (!LoadTexture(device, ambientMaps, this->mapKaTextureArray, this->mapKaSRV, "ambient"))
    {
        return false;
    }
    
    if (!LoadTexture(device, diffuseMaps, this->mapKdTextureArray, this->mapKdSRV, "diffuse"))
    {
        return false;
    }

    if (!LoadTexture(device, specularMaps, this->mapKsTextureArray, this->mapKsSRV, "specular"))
    {
        return false;
    }

    file.close();

    return true;
}

void ObjectHandler::CreateBuffers(ID3D11Device* device)
{
    //Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData{};
    vertexData.pSysMem = vertices.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);

    //Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.ByteWidth = (UINT)(sizeof(int) * indices.size());
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData{};
    indexData.pSysMem = indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);

    //Create middle point constant buffer
    D3D11_BUFFER_DESC constBuffDesc{};
    constBuffDesc.ByteWidth = sizeof(MiddlePoint);
    constBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBuffDesc.MiscFlags = 0;
    constBuffDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA buffData{};
    buffData.pSysMem = &middlePoint;
    buffData.SysMemPitch = 0;
    buffData.SysMemSlicePitch = 0;

    device->CreateBuffer(&constBuffDesc, &buffData, &this->middlePointConstantBuffer);
}

bool ObjectHandler::LoadTexture(ID3D11Device* device, const std::vector<std::string> maps, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, const std::string mapType)
{
    std::vector<unsigned char*> imageDataList;

    int width = 0, height = 0, components = 0;
    for (int i = 0; i < maps.size(); i++)
    {
        unsigned char* imageData = stbi_load(maps[i].c_str(), &width, &height, &components, 4);
        if (imageData == NULL)
        {
            std::string defaultFileName = "Textures/" + mapType + "MissingTexture.png";
            imageData = stbi_load(defaultFileName.c_str(), &width, &height, &components, 4); //Default texture
        }

        imageDataList.push_back(imageData);
    }

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = (UINT)materials.size();
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData{};

    std::vector<D3D11_SUBRESOURCE_DATA> initialDataList;
    for (int i = 0; i < imageDataList.size(); i++)
    {
        initialDataList.push_back(initialData);
        initialDataList[i].pSysMem = imageDataList[i];
        initialDataList[i].SysMemPitch = width * 4;
        initialDataList[i].SysMemSlicePitch = height * width * 4;;
    }

    device->CreateTexture2D(&textureDesc, &initialDataList.data()[0], &texture);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.ArraySize = (UINT)materials.size();
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.MostDetailedMip = 0;

    if (texture != NULL)
    {
        device->CreateShaderResourceView(texture, &srvDesc, &textureSRV);
    }

    for (int i = 0; i < imageDataList.size(); i++)
    {
        stbi_image_free(imageDataList[i]);
    }

    return true;
}

void ObjectHandler::ClearPreviousObject()
{
    if (vertexBuffer != nullptr)
    {
        this->vertexBuffer->Release();
    }

    if (indexBuffer != nullptr)
    {
        this->indexBuffer->Release();
    }

    if (mapKdTextureArray != nullptr)
    {
        this->mapKdTextureArray->Release();
    }

    if (mapKaTextureArray != nullptr)
    {
        this->mapKaTextureArray->Release();
    }

    if (mapKsTextureArray != nullptr)
    {
        this->mapKsTextureArray->Release();
    }

    if (mapKdSRV != nullptr)
    {
        this->mapKdSRV->Release();
    }

    if (mapKaSRV != nullptr)
    {
        this->mapKaSRV->Release();
    }

    if (mapKsSRV != nullptr)
    {
        this->mapKsSRV->Release();
    }

    if (middlePointConstantBuffer != nullptr)
    {
        this->middlePointConstantBuffer->Release();
    }

    this->materials.clear();
    this->vertices.clear();
    this->indices.clear();
    this->objects.clear();
}

ObjectHandler::ObjectHandler()
{
}

ObjectHandler::~ObjectHandler()
{
    this->ClearPreviousObject();
}

bool ObjectHandler::LoadObject(ID3D11Device* device, std::string fileName)
{
    //Clear previous object
    this->ClearPreviousObject();

    //Open the new file
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return false; //File not found
    }

    //Create the necessary variables
    std::string line;
    std::string currentObjectName = "";
    std::string mtlFileName = "";
    int currentMaterial = 0;
    std::vector<XMFLOAT3> positions;
    std::vector<XMFLOAT2> UVs;
    std::vector<XMFLOAT3> normals;

    //Parse each line in the OBJ file
    while (std::getline(file, line))
    {
        std::stringstream ss(line); //Load the line into a stringstream

        //Get the prefix
        std::string prefix;
        ss >> prefix;

        if (prefix == "o")
        {
            //Change currentObjectName
            ss >> currentObjectName;
        }
        else if (prefix == "v")
        {
            //Add vertex positions
            XMFLOAT3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt")
        {
            //Add vertex UVs
            XMFLOAT2 UV;
            ss >> UV.x >> UV.y;
            UVs.push_back(UV);
        }
        else if (prefix == "vn")
        {
            //Add vertex normals
            XMFLOAT3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "usemtl")
        {
            //Add material to list if new. Otherwise select index to give to vertices to keep track of their material.
            std::string materialName;
            ss >> materialName;

            //Check if material already exists
            bool sameMaterial = false;
            for (int i = 0; i < materials.size(); i++)
            {
                if (materials[i].name == materialName)
                {
                    sameMaterial = true;
                    currentMaterial = i;
                    break;
                }
            }

            //Create new material
            if (!sameMaterial)
            {
                currentMaterial = (int)materials.size();
                Material mat =
                {
                    materialName, currentMaterial, 32.0f
                };
                materials.push_back(mat);
            }
        }
        else if (prefix == "f")
        {
            //Isolate the line with all the indices that make up the face
            line = line.substr(2);
            std::stringstream faces(line);
            std::string vertexData;

            //Parse through the line with all indices
            std::vector<int> tempIndices; //Temporary list keeping track of the index for the new vertices to prevent duplicates from being created
            while (std::getline(faces, vertexData, ' '))
            {
                //Isolate each set of vertex indices (position, UV, normal)
                std::stringstream vertexIndices(vertexData);
                std::string indexData;

                //Extract each index as they are separated by a '/' (Example: "1/1/1")
                int posIndex, UVIndex, normalIndex;
                std::getline(vertexIndices, indexData, '/');
                posIndex = std::stoi(indexData) - 1;
                std::getline(vertexIndices, indexData, '/');
                UVIndex = std::stoi(indexData) - 1;
                std::getline(vertexIndices, indexData, ' ');
                normalIndex = std::stoi(indexData) - 1;

                //Create a vertex for this set of indices in the face
                Vertex vertex(positions[posIndex], normals[normalIndex], UVs[UVIndex], currentMaterial);

                //Search for duplicates in current list of vertices
                bool duplicate = false;
                for (int i = 0; i < vertices.size(); i++)
                {
                    if (memcmp(&vertices[i], &vertex, sizeof(Vertex)) == 0) //If the data in vertices[i] perfectly matches the newly created vertex
                    {
                        duplicate = true;
                        tempIndices.push_back(i);
                        break;
                    }
                }
                
                //If no duplicate was found, add the new vertex to the list of vertices
                if (!duplicate)
                {
                    vertices.push_back(vertex);
                    tempIndices.push_back(((int)vertices.size() - 1));
                }
            }

            //Submesh indexCount
            int indexCount = 0;

            //Add the tempIndices to the real list of vertex indices (Based on the amount of new indices)
            if (tempIndices.size() == 3)
            {
                //Add triangular face
                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[1]);
                indices.push_back(tempIndices[2]);

                indexCount = 3;
            }
            else if (tempIndices.size() == 4)
            {
                //Add quadratic face
                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[1]);
                indices.push_back(tempIndices[2]);

                indices.push_back(tempIndices[2]);
                indices.push_back(tempIndices[3]);
                indices.push_back(tempIndices[0]);

                indexCount = 6;
            }
            else if (tempIndices.size() == 5)
            {
                //Add pentagonal face
                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[1]);
                indices.push_back(tempIndices[2]);

                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[2]);
                indices.push_back(tempIndices[3]);

                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[3]);
                indices.push_back(tempIndices[4]);

                indexCount = 9;
            }
            else if (tempIndices.size() == 6)
            {
                //Add hexagonal face
                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[1]);
                indices.push_back(tempIndices[2]);

                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[2]);
                indices.push_back(tempIndices[3]);

                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[3]);
                indices.push_back(tempIndices[4]);

                indices.push_back(tempIndices[0]);
                indices.push_back(tempIndices[4]);
                indices.push_back(tempIndices[5]);

                indexCount = 12;
            }

            //Create or add to an object
            if (!currentObjectName.empty()) //If a name for the object exists
            {
                //Check if an object with the same name already exists
                Object* currentObject = nullptr;
                for (int i = 0; i < objects.size(); i++)
                {
                    if (objects[i].name == currentObjectName)
                    {
                        currentObject = &objects[i];
                        break;
                    }
                }

                //If no object with that name exists, create a new object
                if (currentObject == nullptr)
                {
                    Object newObject;
                    newObject.name = currentObjectName;
                    Submesh submesh;
                    submesh.startIndex = (int)(indices.size() - indexCount);
                    submesh.indexCount = indexCount;
                    newObject.submesh = submesh;
                    objects.push_back(newObject);
                    currentObject = &objects.back();
                }
                else //Update the submesh indexCount
                {
                    currentObject->submesh.indexCount += indexCount;
                }
            }
        }
        if (prefix == "mtllib")
        {
            ss >> mtlFileName;
        }
    }
    file.close();

    //Parse MTL file
    if (!LoadMaterial(device, mtlFileName))
    {
        return false;
    }

    //Give the vertices their proper specularExponent
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i].Ns = materials[vertices[i].textureUsed].specularExponent;
    }

    //Create the buffers
    this->middlePoint.middle = getMiddlePoint();
    CreateBuffers(device);

    return true;
}

int ObjectHandler::getIndexCount() const
{
    return (int)this->indices.size();
}

Submesh ObjectHandler::getSubmesh(int object) const
{
    return this->objects[object].submesh;
}

XMFLOAT3 ObjectHandler::getMiddlePoint() const
{
    XMFLOAT3 sumOfPositions = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < vertices.size(); i++)
    {
        sumOfPositions.x += vertices[i].pos[0] / vertices.size();
        sumOfPositions.y += vertices[i].pos[1] / vertices.size();
        sumOfPositions.z += vertices[i].pos[2] / vertices.size();
    }

    return sumOfPositions;
}

std::vector<Vertex> ObjectHandler::getVertices() const
{
    return this->vertices;
}

std::vector<Object> ObjectHandler::getObjects() const
{
    return this->objects;
}

std::vector<unsigned int> ObjectHandler::getIndices() const
{
    return this->indices;
}