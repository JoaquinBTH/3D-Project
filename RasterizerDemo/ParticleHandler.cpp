#include "ParticleHandler.h"
#include "stb_image.h"

void ParticleHandler::LoadTexture(ID3D11Device* device)
{
    int width = 0, height = 0, components = 0;

    std::string fileName = "Textures/notlikethis_fox.png";

    unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &components, 4);

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

    device->CreateTexture2D(&textureDesc, &initialData, &this->particleTexture);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (this->particleTexture != NULL)
    {
        device->CreateShaderResourceView(this->particleTexture, &srvDesc, &this->particleSRV);
    }

    stbi_image_free(imageData);
}

ParticleHandler::ParticleHandler(ID3D11Device* device, const int nrOfParticles, XMFLOAT3 startPos)
{
	//Create the texture that will be applied to each particle
    LoadTexture(device);

    //Create the particles
    XMFLOAT3 startingPos = startPos;
    XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
    int moveDirection = 0;
    int repeats = 1;
    //Applies a pattern to the particles for fun
    for (int i = 0; i < nrOfParticles; i++)
    {
        XMFLOAT3 particlePos = XMFLOAT3(startingPos.x + offset.x, startingPos.y + offset.y, startingPos.z + offset.z);

        particles.push_back(Particle(particlePos));

        if (i % 2 == 0)
        {
            //Change direction and offset X
            if (offset.x <= 0.0f)
            {
                offset.x = -offset.x + 0.2f;
            }
            else
            {
                offset.x = -offset.x - 0.2f;
            }

            //Offset Y and change to 0 and move completely if below or equal to -5
            offset.y -= 0.2f;
            if (offset.y <= -5.0f)
            {
                offset.y = 0.0f;

                if (moveDirection == 0)
                {
                    //Move Positive X
                    startingPos = XMFLOAT3(startingPos.x + abs(offset.x) * 2 * repeats, startingPos.y, startingPos.z + abs(offset.z) * 2 * (repeats - 1));
                    offset.x = 0.0f;
                    offset.z = 0.0f;
                    moveDirection++;
                }
                else if (moveDirection == 1)
                {
                    //Move Negative X
                    startingPos = XMFLOAT3(startingPos.x - abs(offset.x) * 4 * repeats, startingPos.y, startingPos.z);
                    offset.x = 0.0f;
                    offset.z = 0.0f;
                    moveDirection++;
                }
                else if (moveDirection == 2)
                {
                    //Move Positive Z
                    startingPos = XMFLOAT3(startingPos.x + abs(offset.x) * 2 * repeats, startingPos.y, startingPos.z + abs(offset.z) * 2 * repeats);
                    offset.x = 0.0f;
                    offset.z = 0.0f;
                    moveDirection++;
                }
                else if (moveDirection == 3)
                {
                    //Move Negative Z
                    startingPos = XMFLOAT3(startingPos.x, startingPos.y, startingPos.z - abs(offset.z) * 4 * repeats);
                    offset.x = 0.0f;
                    offset.z = 0.0f;
                    moveDirection = 0;
                    repeats++;
                }
            }
        }
        else
        {
            //Change direction and offset Z
            if (offset.z <= 0.0f)
            {
                offset.z = -offset.z + 0.2f;
            }
            else
            {
                offset.z = -offset.z - 0.2f;
            }
        }
    }

    //Create the structured buffer
    D3D11_BUFFER_DESC buffDesc{};
    buffDesc.ByteWidth = sizeof(Particle) * nrOfParticles;
    buffDesc.Usage = D3D11_USAGE_DEFAULT;
    buffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buffDesc.StructureByteStride = sizeof(Particle);

    D3D11_SUBRESOURCE_DATA initialData{};
    initialData.pSysMem = &particles[0];
    initialData.SysMemPitch = 0;
    initialData.SysMemSlicePitch = 0;

    D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc{};
    UAVdesc.Format = DXGI_FORMAT_UNKNOWN;
    UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    UAVdesc.Buffer.FirstElement = 0;
    UAVdesc.Buffer.NumElements = nrOfParticles;
    UAVdesc.Buffer.Flags = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc{};
    SRVdesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVdesc.Buffer.FirstElement = 0;
    SRVdesc.Buffer.NumElements = nrOfParticles;
    SRVdesc.Buffer.ElementOffset = 0;

    device->CreateBuffer(&buffDesc, &initialData, &this->particleBuffer);
    if (this->particleBuffer != NULL)
    {
        device->CreateUnorderedAccessView(this->particleBuffer, &UAVdesc, &this->particleStructuredUAV);
        device->CreateShaderResourceView(this->particleBuffer, &SRVdesc, &this->particleStructuredSRV);
    }
}

ParticleHandler::~ParticleHandler()
{
    this->particles.clear();

    if (particleVertexShader) particleVertexShader->Release();
    if(particleGeometryShader) particleGeometryShader->Release();
    if(particleComputeShader) particleComputeShader->Release();
    if(particlePixelShader) particlePixelShader->Release();

    if(particleTexture) particleTexture->Release();
    if(particleSRV) particleSRV->Release();

    if(particleBuffer) particleBuffer->Release();
    if (particleInputLayout) particleInputLayout->Release();
    if(particleStructuredSRV) particleStructuredSRV->Release();
    if(particleStructuredUAV) particleStructuredUAV->Release();
}
