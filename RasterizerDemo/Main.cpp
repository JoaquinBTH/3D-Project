#include "Camera.h"
#include "Timer.h"
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
//#include "Octree.h"
#include "Render.h"

#include "imgui\imgui.h"
#include "imgui\backends\imgui_impl_win32.h"
#include "imgui\backends\imgui_impl_dx11.h"

#include "ImGuiHelper.h"

void UpdateCamConstBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer*& updateConstantBuffer, CameraPos* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	immediateContext->Map(updateConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, sizeof(CameraPos));
	immediateContext->Unmap(updateConstantBuffer, 0);
}

void UpdateConstBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer*& updateConstantBuffer, MatrixConstantBuffer* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	immediateContext->Map(updateConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, sizeof(MatrixConstantBuffer));
	immediateContext->Unmap(updateConstantBuffer, 0);
}

void UpdateCamera(Camera*& camera, float dt)
{
	if (GetAsyncKeyState('W'))
	{
		camera->AdjustPosition(camera->GetForwardVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState('S'))
	{
		camera->AdjustPosition(camera->GetBackwardVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState('A'))
	{
		camera->AdjustPosition(camera->GetLeftVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState('D'))
	{
		camera->AdjustPosition(camera->GetRightVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		camera->AdjustPosition(0, 0.005f * dt, 0);
	}
	if (GetAsyncKeyState(VK_SHIFT))
	{
		camera->AdjustPosition(0, -0.005f * dt, 0);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		camera->AdjustRotation(-0.002f * dt, 0, 0);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		camera->AdjustRotation(0.002f * dt, 0, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		camera->AdjustRotation(0, 0.002f * dt, 0);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		camera->AdjustRotation(0, -0.002f * dt, 0);
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	const UINT WIDTH = 1024; //Width of window
	const UINT HEIGHT = 512; //Height of window
	HWND window; //Define a window handler.

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	//Timer for delta value.
	Timer timer;

	//Create Renderer.
	Render* renderer = new Render();

	//Standard Rendering
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11SamplerState* sampler;

	//Shadow Mapping
	ID3D11VertexShader* vShaderShadow;
	ID3D11PixelShader* pShaderShadow;

	//Set up the environment
	if (!SetupD3D11(WIDTH, HEIGHT, window, renderer->device, renderer->immediateContext, renderer->swapChain, renderer->rtv, renderer->uav, renderer->dsTexture, renderer->dsView, renderer->viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	//Create the camera
	Camera* camera = new Camera(renderer->device, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT));

	//Create world/view+proj matrices
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewAndProjectionMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	MatrixConstantBuffer matrices =
	{
		worldMatrix, viewAndProjectionMatrix
	};
	ID3D11Buffer* matrixConstantBuffer;

	//Create the lights
	LightHandler* lights = new LightHandler();
	lights->AddLight(renderer->device, 0);
	lights->AddLight(renderer->device, 0, XMFLOAT3(10.0f, 4.5f, -4.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), 10.0f);
	lights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, -1.0f));
	lights->AddLight(renderer->device, 0, XMFLOAT3(6.8f, 10.0f, 5.5f), XMFLOAT3(0.0f, -1.0f, 0.00001f), 20.0f);
	lights->AddLight(renderer->device, 0, XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.00001f), 40.0f);

	//Create object
	ObjectHandler* object = new ObjectHandler();
	object->LoadObject(renderer->device, "Models/Room.obj");

	//Deferred rendering
	DeferredHandler* deferred = new DeferredHandler(renderer->device, WIDTH, HEIGHT);

	//Level of detail tessellation and Object
	ObjectHandler* LODobject = new ObjectHandler();
	LODobject->LoadObject(renderer->device, "Models/TessellationPlane.obj");
	LODHandler* LOD = new LODHandler(renderer->device, "Wall");

	//Cube mapping
	CubeMapHandler* cubeMap = new CubeMapHandler(renderer->device, WIDTH);

	//Octree Culling
	OctreeHandler* octree = new OctreeHandler(renderer->device, "Models/CullingScene.obj", XMFLOAT3(0.0f, 0.0f, 0.0f), 0, 100.0f); //Change depth from 0 to 3
	LightHandler* octreeLights = new LightHandler();
	octreeLights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
	octreeLights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	octreeLights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	octreeLights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	octreeLights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	octreeLights->AddLight(renderer->device, 1, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

	//Particles
	ParticleHandler* particles = new ParticleHandler(renderer->device, 867); //255 particles for first pattern, then add 204 per extention (51 particles per pyramid)

	//Set up ImGui
	SetupImGui(window, renderer->device, renderer->immediateContext);

	//Initialize the pipeline
	if (!SetupPipeline(renderer->device, vShader, vShaderShadow, pShader, pShaderShadow, renderer->inputLayout, matrixConstantBuffer, sampler, deferred, LOD, cubeMap, particles))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	//Use method switches
	bool useDeferred = false;
	bool useLOD = false;
	bool useCubeMap = false;
	bool useCulling = false;
	bool useParticle = false;

	MSG msg = { };

	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Timer
		float dt = (float)timer.GetElapsedTime();
		timer.Restart();

		//Update Camera
		UpdateCamera(camera, dt);
		UpdateCamConstBuffer(renderer->immediateContext, camera->cameraPosConstantBuffer, &camera->camPos);

		//Rotate the scene around the Y axis (based on the time passed since the start of the program)
		//worldMatrix *= XMMatrixRotationY(0.0002f * dt);

		//Update Matrices
		viewAndProjectionMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
		XMStoreFloat4x4(&matrices.viewAndProjectionMatrix, viewAndProjectionMatrix);
		XMStoreFloat4x4(&matrices.worldMatrix, worldMatrix);
		UpdateConstBuffer(renderer->immediateContext, matrixConstantBuffer, &matrices);

		StartImGuiFrame();

		if (useDeferred == true)
		{
			renderer->ShadowMap(object, lights, vShaderShadow, pShaderShadow, matrixConstantBuffer);
			renderer->DeferredRender(object, deferred, matrixConstantBuffer, sampler, lights, camera->cameraPosConstantBuffer);
		}
		else if (useCubeMap == true)
		{
			renderer->ShadowMap(object, lights, vShaderShadow, pShaderShadow, matrixConstantBuffer);
			renderer->CubeRender(object, cubeMap, vShader, pShader, matrixConstantBuffer, sampler, lights, camera->cameraPosConstantBuffer);
		}
		else if (useLOD == true)
		{
			renderer->ShadowMap(LODobject, lights, vShaderShadow, pShaderShadow, matrixConstantBuffer);
			renderer->LODRender(LODobject, LOD, matrixConstantBuffer, sampler, lights, camera->cameraPosConstantBuffer);
		}
		else if (useCulling == true)
		{
			octree->FrustumCulling(camera);
			renderer->ShadowMap(octree->scene, octreeLights, vShaderShadow, pShaderShadow, matrixConstantBuffer);
			renderer->CullingRender(octree, vShader, pShader, sampler, octreeLights, matrixConstantBuffer, camera->cameraPosConstantBuffer);
			ImGuiObjectsRendered((int)octree->objectIndices.size());
		}
		else if (useParticle == true)
		{
			renderer->ParticleRender(particles, sampler, matrixConstantBuffer, camera->cameraPosConstantBuffer);
		}
		else
		{
			renderer->ShadowMap(object, lights, vShaderShadow, pShaderShadow, matrixConstantBuffer);
			renderer->StandardRender(object, vShader, pShader, matrixConstantBuffer, sampler, lights, camera->cameraPosConstantBuffer);
		}
		ImGuiSelectRenderMethod(useDeferred, useCubeMap, useLOD, useCulling, useParticle);

		EndImGuiFrame();

		renderer->swapChain->Present(0, 0); //Swap the front buffer with the back buffer.
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//Release all COM objects
	pShader->Release();
	vShader->Release();
	vShaderShadow->Release();
	pShaderShadow->Release();
	sampler->Release();
	matrixConstantBuffer->Release();
	delete renderer;
	delete lights;
	delete camera;
	delete object;
	delete LODobject;
	delete deferred;
	delete LOD;
	delete cubeMap;
	delete octree;
	delete octreeLights;
	delete particles;

	return 0;
}