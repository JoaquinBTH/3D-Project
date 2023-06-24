#include "Render.h"

void Render::ClearBindings()
{
	//Clear the input assembly
	ID3D11Buffer* nullBuffer = nullptr;
	UINT stride = 0;
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	immediateContext->IASetInputLayout(nullptr);

	//Clear the VS and PS
	ID3D11VertexShader* nullVS = nullptr;
	ID3D11PixelShader* nullPS = nullptr;
	immediateContext->VSSetShader(nullVS, nullptr, 0);
	immediateContext->PSSetShader(nullPS, nullptr, 0);

	//Clear the RTV and DSV
	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11DepthStencilView* nullDSV = nullptr;
	immediateContext->OMSetRenderTargets(1, &nullRTV, nullDSV);

	//Clear the SRV
	ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };
	immediateContext->VSSetShaderResources(0, 3, nullSRV);
	immediateContext->PSSetShaderResources(0, 3, nullSRV);

	//Clear the Sampler
	ID3D11SamplerState* nullSampler = nullptr;
	immediateContext->PSSetSamplers(0, 1, &nullSampler);
}

Render::Render()
{
}

Render::~Render()
{
	device->Release();
	immediateContext->Release();
	swapChain->Release();
	dsTexture->Release();
	dsView->Release();
	inputLayout->Release();
	rtv->Release();
}

void Render::ShadowMap(ObjectHandler* object, LightHandler* lights, ID3D11VertexShader* vShaderShadow, ID3D11PixelShader* pShaderShadow, ID3D11Buffer* matrixConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	for (int i = 0; i < lights->getNrOfLights(); i++)
	{
		immediateContext->ClearDepthStencilView(lights->lightSMV[i].shadowMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->VSSetShader(vShaderShadow, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShaderShadow, nullptr, 0);
	
	//Create the Shadow Maps from the light's perspective
	for (int i = 0; i < lights->getNrOfLights(); i++)
	{
		immediateContext->OMSetRenderTargets(0, NULL, lights->lightSMV[i].shadowMapDSV);
		immediateContext->VSSetConstantBuffers(1, 1, &lights->lightSMV[i].individualLightConstantBuffer);
		immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);
	}
}

void Render::StandardRender(ObjectHandler* object, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &lights->numberOfLightsBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &object->mapKdSRV);
	immediateContext->PSSetShaderResources(1, 1, &lights->shadowMapSRV);
	immediateContext->PSSetShaderResources(2, 1, &lights->lightSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);
}

/*
void Render::StandardRender(ID3D11ShaderResourceView* textureSRV, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* vertexBuffer, ID3D11Buffer* matrixConstantBuffer, Parser* baseModel, ID3D11SamplerState* sampler, ID3D11ShaderResourceView* lightSRV, ID3D11Buffer* numberOfLightsBuffer, ID3D11Buffer* cameraPosConstantBuffer)
{
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &numberOfLightsBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &textureSRV);
	immediateContext->PSSetShaderResources(1, 1, &lightSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	immediateContext->Draw(baseModel->AmountOfVertices(), 0);
}
*/

/*
void Render::CubeRender(ID3D11Buffer* vertexBuffer, ID3D11Buffer* currentLightConstBuff[4], ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cubeConstBuffer[6], ID3D11Buffer* cameraPosConstantBuffer, ID3D11Buffer* cubeVertexBuffer)
{
}

void Render::CullingRender(ID3D11Buffer* matrixConstantBuffer, std::vector<int> marked, ID3D11Buffer* frustumVertexBuffer[50], ID3D11ShaderResourceView* frustumSRV[50], std::vector<Parser*>& frustumObjects)
{
}

void Render::LODRender(ID3D11Buffer* LODBuffer, ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer)
{
}

void Render::ParticleRender(ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer)
{
}
*/
