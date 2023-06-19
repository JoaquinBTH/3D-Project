#include "Camera.h"
//#include "Light.h"
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

void UpdateCamera(Camera& camera, float dt)
{
	if (GetAsyncKeyState('W'))
	{
		camera.AdjustPosition(camera.GetForwardVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState('S'))
	{
		camera.AdjustPosition(camera.GetBackwardVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState('A'))
	{
		camera.AdjustPosition(camera.GetLeftVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState('D'))
	{
		camera.AdjustPosition(camera.GetRightVector() * 0.005f * dt);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		camera.AdjustPosition(0, 0.005f * dt, 0);
	}
	if (GetAsyncKeyState(VK_SHIFT))
	{
		camera.AdjustPosition(0, -0.005f * dt, 0);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		camera.AdjustRotation(-0.002f * dt, 0, 0);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		camera.AdjustRotation(0.002f * dt, 0, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		camera.AdjustRotation(0, 0.002f * dt, 0);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		camera.AdjustRotation(0, -0.002f * dt, 0);
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

	//Camera
	Camera camera;
	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.SetProjectionValues(90.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 1000.0f);
	CameraPos camPos =
	{
		camera.GetPositionFloat3()
	};
	ID3D11Buffer* cameraPosConstantBuffer;

	//Create Renderer.
	Render* renderer = new Render();

	//Standard Rendering
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;

	//Base model and sampler
	ID3D11Buffer* vertexBuffer;
	Parser* baseModel = new Parser("Models/Room.obj");
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11SamplerState* sampler;

	//Create world/view+proj matrices
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewAndProjectionMatrix = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	MatrixConstantBuffer matrices =
	{
		worldMatrix, viewAndProjectionMatrix
	};
	ID3D11Buffer* matrixConstantBuffer;

	//Set up the environment
	if (!SetupD3D11(WIDTH, HEIGHT, window, renderer->device, renderer->immediateContext, renderer->swapChain, renderer->rtv, renderer->dsTexture, renderer->dsView, renderer->viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -1;
	}

	//Create the lights
	LightHandler* lights = new LightHandler();
	lights->AddLight(renderer->device, 0);
	lights->AddLight(renderer->device, 1, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, -1.0f));

	//Set up ImGui
	SetupImGui(window, renderer->device, renderer->immediateContext);

	//Initialize the pipeline
	if (!SetupPipeline(renderer->device, vertexBuffer, vShader, pShader, renderer->inputLayout, matrixConstantBuffer, baseModel, texture, textureSRV, sampler, cameraPosConstantBuffer))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -1;
	}

	//Use method switches
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
		camPos.cameraPos = camera.GetPositionFloat3();
		UpdateCamConstBuffer(renderer->immediateContext, cameraPosConstantBuffer, &camPos);

		//Rotate the scene around the Y axis (based on the time passed since the start of the program)
		//worldMatrix *= XMMatrixRotationY(0.0002f * dt);

		//Update Matrices
		viewAndProjectionMatrix = camera.GetViewMatrix() * camera.GetProjectionMatrix();
		XMStoreFloat4x4(&matrices.viewAndProjectionMatrix, viewAndProjectionMatrix);
		XMStoreFloat4x4(&matrices.worldMatrix, worldMatrix);
		UpdateConstBuffer(renderer->immediateContext, matrixConstantBuffer, &matrices);

		StartImGuiFrame();

		if (useCubeMap == true)
		{
			//renderer->CubeRender(vertexBuffer, currentLightConstBuff, matrixConstantBuffer, cubeConstantBuffer, cameraPosConstantBuffer, cubeVertexBuffer);
		}
		else if (useLOD == true)
		{
			//renderer->LODRender(LODBuffer, matrixConstantBuffer, cameraPosConstantBuffer);
		}
		else if (useCulling == true)
		{
			//renderer->CullingRender(matrixConstantBuffer, marked, frustumVertexBuffer, frustumSRV, frustumObjects);
		}
		else if (useParticle == true)
		{
			//renderer->ParticleRender(matrixConstantBuffer, cameraPosConstantBuffer);
		}
		else
		{
			//renderer->StandardRender(vertexBuffer, indexBufferTest, currentLightConstBuff, matrixConstantBuffer, cameraPosConstantBuffer);
			renderer->StandardRender(textureSRV, vShader, pShader, vertexBuffer, matrixConstantBuffer, baseModel, sampler, lights->lightSRV, lights->numberOfLightsBuffer, cameraPosConstantBuffer);
		}
		ImGuiSelectRenderMethod(useCubeMap, useLOD, useCulling, useParticle);

		EndImGuiFrame();

		renderer->swapChain->Present(0, 0); //Swap the front buffer with the back buffer.
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//Release all COM objects
	vertexBuffer->Release();
	pShader->Release();
	vShader->Release();
	texture->Release();
	textureSRV->Release();
	sampler->Release();
	matrixConstantBuffer->Release();
	cameraPosConstantBuffer->Release();
	delete renderer;
	delete baseModel;
	delete lights;

	return 0;
}