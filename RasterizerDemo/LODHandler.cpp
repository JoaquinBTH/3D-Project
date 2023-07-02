#include "LODHandler.h"
#include "stb_image.h"

bool LODHandler::LoadTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, std::string mapName, bool normal)
{
    int width = 0, height = 0, components = 0;

    unsigned char* imageData = stbi_load(mapName.c_str(), &width, &height, &components, 4);
    if (imageData == NULL)
    {
        std::string defaultFileName;
        if (normal)
        {
            defaultFileName = "Textures/WallNormal.png";
        }
        else
        {
            defaultFileName = "Textures/WallHeight.png";
        }
        imageData = stbi_load(defaultFileName.c_str(), &width, &height, &components, 4); //Default texture
    }

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = imageData;
    initialData.SysMemPitch = width * 4;
    initialData.SysMemSlicePitch = height * width * 4;

    device->CreateTexture2D(&textureDesc, &initialData, &texture);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (texture != NULL)
    {
        device->CreateShaderResourceView(texture, &srvDesc, &textureSRV);
    }

    stbi_image_free(imageData);

    return true;
}

void LODHandler::ClearPreviousTexturesAndRaster()
{
    if (heightTexture) heightTexture->Release();
    if (normalTexture) normalTexture->Release();
    if (heightSRV) heightSRV->Release();
    if (normalSRV) normalSRV->Release();
    if (LODRaster) LODRaster->Release();
}

LODHandler::LODHandler(ID3D11Device* device, std::string mapName)
{
    ClearPreviousTexturesAndRaster();
    std::string mapFullName = "Textures/" + mapName + "Height.png";
    LoadTexture(device, heightTexture, heightSRV, mapFullName, false);
    mapFullName = "Textures/" + mapName + "Normal.png";
    LoadTexture(device, normalTexture, normalSRV, mapFullName, true);

    D3D11_RASTERIZER_DESC rDesc{};
    rDesc.FillMode = D3D11_FILL_WIREFRAME;
    rDesc.CullMode = D3D11_CULL_BACK;
    rDesc.FrontCounterClockwise = false;

    device->CreateRasterizerState(&rDesc, &this->LODRaster);
}

LODHandler::~LODHandler()
{
	if (LODvShader) LODvShader->Release();
	if (LODhShader) LODhShader->Release();
	if (LODdShader) LODdShader->Release();
	if (LODRaster) LODRaster->Release();

	if (heightTexture) heightTexture->Release();
    if (normalTexture) normalTexture->Release();
    if (heightSRV) heightSRV->Release();
	if (normalSRV) normalSRV->Release();
}