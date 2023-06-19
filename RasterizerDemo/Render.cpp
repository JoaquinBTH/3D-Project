#include "Render.h"

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
