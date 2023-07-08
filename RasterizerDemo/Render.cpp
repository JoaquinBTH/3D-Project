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
	ID3D11RenderTargetView* nullRTVs[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	ID3D11DepthStencilView* nullDSV = nullptr;
	immediateContext->OMSetRenderTargets(6, nullRTVs, nullDSV);

	//Clear the SRV
	ID3D11ShaderResourceView* nullSRV[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	immediateContext->VSSetShaderResources(0, 5, nullSRV);
	immediateContext->PSSetShaderResources(0, 5, nullSRV);

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
	uav->Release();
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

	//Clear backbuffer
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//Input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;	
	immediateContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Viewport
	immediateContext->RSSetViewports(1, &viewport);

	//Vertex Shader
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Pixel Shader
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &lights->numberOfLightsBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &object->mapKaSRV);
	immediateContext->PSSetShaderResources(1, 1, &object->mapKdSRV);
	immediateContext->PSSetShaderResources(2, 1, &object->mapKsSRV);
	immediateContext->PSSetShaderResources(3, 1, &lights->shadowMapSRV);
	immediateContext->PSSetShaderResources(4, 1, &lights->lightSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);

	//Output Merger
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Draw
	immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);
}

void Render::DeferredRender(ObjectHandler* object, DeferredHandler* deferred, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	
	//Clear G-Buffers
	immediateContext->ClearRenderTargetView(deferred->positionRTV, clearColour);
	immediateContext->ClearRenderTargetView(deferred->normalRTV, clearColour);
	immediateContext->ClearRenderTargetView(deferred->ambientRTV, clearColour);
	immediateContext->ClearRenderTargetView(deferred->diffuseRTV, clearColour);
	immediateContext->ClearRenderTargetView(deferred->specularRTV, clearColour);
	immediateContext->ClearRenderTargetView(deferred->shininessRTV, clearColour);

	//Input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Viewport
	immediateContext->RSSetViewports(1, &viewport);

	//Vertex Shader
	immediateContext->VSSetShader(deferred->deferredVertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Pixel Shader
	immediateContext->PSSetShader(deferred->deferredPixelShader, nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, &object->mapKaSRV);
	immediateContext->PSSetShaderResources(1, 1, &object->mapKdSRV);
	immediateContext->PSSetShaderResources(2, 1, &object->mapKsSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);

	//Output Merger
	ID3D11RenderTargetView* RTVs[6];
	RTVs[0] = deferred->positionRTV;
	RTVs[1] = deferred->normalRTV;
	RTVs[2] = deferred->ambientRTV;
	RTVs[3] = deferred->diffuseRTV;
	RTVs[4] = deferred->specularRTV;
	RTVs[5] = deferred->shininessRTV;
	immediateContext->OMSetRenderTargets(6, RTVs, dsView);

	//Draw
	immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);

	//Unbind resources
	this->ClearBindings();

	//Compute Shader
	ID3D11ShaderResourceView* SRVs[8];
	SRVs[0] = deferred->positionSRV;
	SRVs[1] = deferred->normalSRV;
	SRVs[2] = deferred->ambientSRV;
	SRVs[3] = deferred->diffuseSRV;
	SRVs[4] = deferred->specularSRV;
	SRVs[5] = deferred->shininessSRV;
	SRVs[6] = lights->shadowMapSRV;
	SRVs[7] = lights->lightSRV;
	immediateContext->CSSetShader(deferred->deferredComputeShader, nullptr, 0);
	immediateContext->CSSetShaderResources(0, 8, SRVs);
	immediateContext->CSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->CSSetConstantBuffers(1, 1, &lights->numberOfLightsBuffer);
	immediateContext->CSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

	immediateContext->Dispatch(128, 64, 1);

	//Unbind resources
	ID3D11ShaderResourceView* nullSRVs[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	immediateContext->CSSetShaderResources(0, 8, nullSRVs);
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	immediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	ID3D11ComputeShader* nullCS = nullptr;
	immediateContext->CSSetShader(nullCS, nullptr, 0);

	//Bind the RTV for ImGui
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);
}

void Render::LODRender(ObjectHandler* object, LODHandler* LOD, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	//Clear backbuffer
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//Input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	//Viewport
	immediateContext->RSSetViewports(1, &viewport);

	//RasterState
	ID3D11RasterizerState* nullRS = nullptr;
	//immediateContext->RSSetState(LOD->LODRaster);

	//Vertex Shader
	immediateContext->VSSetShader(LOD->LODvShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->VSSetConstantBuffers(1, 1, &object->middlePointConstantBuffer);
	immediateContext->VSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);

	//Hull Shader
	ID3D11HullShader* nullHS = nullptr;
	immediateContext->HSSetShader(LOD->LODhShader, nullptr, 0);

	//DomainShader
	ID3D11DomainShader* nullDS = nullptr;
	immediateContext->DSSetShader(LOD->LODdShader, nullptr, 0);
	immediateContext->DSSetShaderResources(0, 1, &LOD->heightSRV);
	immediateContext->DSSetShaderResources(1, 1, &LOD->normalSRV);
	immediateContext->DSSetSamplers(0, 1, &sampler);
	immediateContext->DSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Pixel Shader
	immediateContext->PSSetShader(LOD->LODpShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &lights->numberOfLightsBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &object->mapKaSRV);
	immediateContext->PSSetShaderResources(1, 1, &object->mapKdSRV);
	immediateContext->PSSetShaderResources(2, 1, &object->mapKsSRV);
	immediateContext->PSSetShaderResources(3, 1, &lights->shadowMapSRV);
	immediateContext->PSSetShaderResources(4, 1, &lights->lightSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);

	//Output Merger
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Draw
	immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);

	//Unbind Raster, hull shader, and domain shader
	immediateContext->RSSetState(nullRS);
	immediateContext->HSSetShader(nullHS, nullptr, 0);
	immediateContext->DSSetShader(nullDS, nullptr, 0);
}

void Render::CubeRender(ObjectHandler* object, CubeMapHandler* cubeMap, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	//Clear backbuffer
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	for (int i = 0; i < 6; i++)
	{
		immediateContext->ClearRenderTargetView(cubeMap->cubeRTV[i], clearColour);
	}
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//Input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &object->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Viewport
	immediateContext->RSSetViewports(1, &cubeMap->cubeport);

	//Vertex Shader
	immediateContext->VSSetShader(cubeMap->cubeVertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Pixel Shader
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &lights->numberOfLightsBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &object->mapKaSRV);
	immediateContext->PSSetShaderResources(1, 1, &object->mapKdSRV);
	immediateContext->PSSetShaderResources(2, 1, &object->mapKsSRV);
	immediateContext->PSSetShaderResources(3, 1, &lights->shadowMapSRV);
	immediateContext->PSSetShaderResources(4, 1, &lights->lightSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);

	//Render the scene to fill the cubeMap RTVs
	for (int i = 0; i < 6; i++)
	{
		immediateContext->VSSetConstantBuffers(1, 1, &cubeMap->cubeConstBuff[i]);

		//Output Merger
		immediateContext->OMSetRenderTargets(1, &cubeMap->cubeRTV[i], NULL);

		//Draw
		immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);
	}

	//Bind new vertex shader and render the scene normally
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Output Merger
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Change Viewport
	immediateContext->RSSetViewports(1, &viewport);

	//Draw
	immediateContext->DrawIndexed(object->getIndexCount(), 0, 0);

	//Bind a new pixel shader and render the cubeMap
	immediateContext->PSSetShader(cubeMap->cubePixelShader, nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, &cubeMap->cubeSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->PSSetConstantBuffers(0, 1, &cameraPosConstantBuffer);

	//Set the new vertex buffer and index buffer
	immediateContext->IASetVertexBuffers(0, 1, &cubeMap->cubeMapObject->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(cubeMap->cubeMapObject->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set Output Merger, but no depth stencil
	immediateContext->OMSetRenderTargets(1, &rtv, NULL);

	//Draw the cube map object
	immediateContext->DrawIndexed(cubeMap->cubeMapObject->getIndexCount(), 0, 0);
}

void Render::CullingRender(OctreeHandler* octree, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	//Clear backbuffer
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//Input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &octree->scene->vertexBuffer, &stride, &offset);
	immediateContext->IASetIndexBuffer(octree->scene->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Viewport
	immediateContext->RSSetViewports(1, &viewport);

	//Vertex Shader
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Pixel Shader
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &matrixConstantBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &lights->numberOfLightsBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &cameraPosConstantBuffer);
	immediateContext->PSSetShaderResources(0, 1, &octree->scene->mapKaSRV);
	immediateContext->PSSetShaderResources(1, 1, &octree->scene->mapKdSRV);
	immediateContext->PSSetShaderResources(2, 1, &octree->scene->mapKsSRV);
	immediateContext->PSSetShaderResources(3, 1, &lights->shadowMapSRV);
	immediateContext->PSSetShaderResources(4, 1, &lights->lightSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);

	//Output Merger
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Only draw the objects inside the view-frustum
	for (int i = 0; i < octree->objectIndices.size(); i++)
	{
		immediateContext->DrawIndexed(octree->scene->getSubmesh(octree->objectIndices[i]).indexCount, octree->scene->getSubmesh(octree->objectIndices[i]).startIndex, 0);
	}
}

void Render::ParticleRender(ParticleHandler* particles, ID3D11SamplerState* sampler, ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer)
{
	//Unbind everything that's necessary
	this->ClearBindings();

	//Clear backbuffer
	float clearColour[4] = { 0, 0, 0, 0 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//Input assembler
	immediateContext->IASetInputLayout(particles->particleInputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	//Viewport
	immediateContext->RSSetViewports(1, &viewport);

	//Compute shader to update each particle
	immediateContext->CSSetShader(particles->particleComputeShader, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &particles->particleStructuredUAV, 0);
	immediateContext->Dispatch(UINT(particles->particles.size()), 1, 1);

	//Unbind Compute shader and UAV
	ID3D11ComputeShader* nullCS = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	immediateContext->CSSetShader(nullCS, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);

	//Vertex Shader
	immediateContext->VSSetShader(particles->particleVertexShader, nullptr, 0);
	immediateContext->VSSetShaderResources(0, 1, &particles->particleStructuredSRV);
	immediateContext->VSSetConstantBuffers(0, 1, &matrixConstantBuffer);

	//Geometry Shader
	immediateContext->GSSetShader(particles->particleGeometryShader, nullptr, 0);
	immediateContext->GSSetConstantBuffers(0, 1, &cameraPosConstantBuffer);
	immediateContext->GSSetConstantBuffers(1, 1, &matrixConstantBuffer);

	//Pixel Shader
	immediateContext->PSSetShader(particles->particlePixelShader, nullptr, 0);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->PSSetShaderResources(0, 1, &particles->particleSRV);

	//Output Merger
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	//Draw
	immediateContext->Draw(UINT(particles->particles.size()), 0);

	//Unbind Geometry Shader
	ID3D11GeometryShader* nullGS = nullptr;
	immediateContext->GSSetShader(nullGS, nullptr, 0);
}