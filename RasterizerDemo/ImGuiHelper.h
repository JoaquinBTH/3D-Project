#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "PipelineHelper.h"

void SetupImGui(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

void StartImGuiFrame();

void ImGuiExample(ID3D11ShaderResourceView* secretImageSRV, int secretImageWidth, int secretImageHeight);

void ImGuiModifying(CameraPos* toModify, ID3D11DeviceContext* context, ID3D11Buffer* buffer);

void ImGuiSelectRenderMethod(bool& useCubeMap, bool& useLOD, bool& useCulling, bool& useParticle);

void EndImGuiFrame();