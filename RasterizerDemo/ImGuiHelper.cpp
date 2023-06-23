#include "ImGuiHelper.h"

#include "imgui\imgui.h"
#include "imgui\backends\imgui_impl_win32.h"
#include "imgui\backends\imgui_impl_dx11.h"

void SetupImGui(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(windowHandle);
	ImGui_ImplDX11_Init(device, deviceContext);
}

void StartImGuiFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiExample(ID3D11ShaderResourceView* secretImageSRV, int secretImageWidth, int secretImageHeight)
{
	static bool active = true;
	static bool secretMode = false;
	ImGui::Begin("My First Tool", &active, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..")) { active = true; }
			if (ImGui::MenuItem("Close")) { active = false; }
			if (ImGui::MenuItem("Secret stuff")) { secretMode = true; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (secretMode)
	{
		bool begun = ImGui::Begin("Secret stuff");
		if (begun)
			ImGui::Image(secretImageSRV, ImVec2(static_cast<float>(secretImageWidth), static_cast<float>(secretImageHeight)));

		ImGui::End();
	}

	if (active)
	{
		// Edit a color (stored as ~4 floats)
		static float my_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		ImGui::ColorEdit4("My fancy colour picker", my_color);

		// Plot some values
		const float my_values[] = { 5.0f, 6.0f, 7.0f, 8.0f, 20.0f, 2.0f, 2.0f };
		ImGui::PlotLines("Number of coffe cups Marcus drinks during a work week", my_values, sizeof(my_values) / sizeof(float), 0, "", 0, 25.0f, ImVec2(500, 200));

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; n++)
			ImGui::Text("%04d: Some text", n);
		ImGui::EndChild();
	}

	ImGui::End();
}

void ImGuiSelectRenderMethod(bool& useCubeMap, bool& useLOD, bool& useCulling, bool& useParticle)
{
	bool begun = ImGui::Begin("Render Method");
	if (begun)
	{
		if(ImGui::Button("Standard"))
		{
			useCubeMap = false;
			useLOD = false;
			useCulling = false;
			useParticle = false;
		}
		else if (ImGui::Button("Cube Mapping"))
		{
			useCubeMap = true;
			useLOD = false;
			useCulling = false;
			useParticle = false;
		}
		else if (ImGui::Button("Level of Detail"))
		{
			useCubeMap = false;
			useLOD = true;
			useCulling = false;
			useParticle = false;
		}
		else if (ImGui::Button("Octree Culling"))
		{
			useCubeMap = false;
			useLOD = false;
			useCulling = true;
			useParticle = false;
		}
		else if (ImGui::Button("Particles"))
		{
			useCubeMap = false;
			useLOD = false;
			useCulling = false;
			useParticle = true;
		}
	}
	ImGui::End();
}

void EndImGuiFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
