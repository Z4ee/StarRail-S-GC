#pragma once

#include "device.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);

namespace Device
{
	using PresentFn = HRESULT(__stdcall*) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	using ResizeBuffersFn = HRESULT(__stdcall*) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

	PresentFn oPresent = nullptr;
	ResizeBuffersFn oResizeBuffers = nullptr;

	LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (globals::Unload)
			return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
		if (uMsg == WM_KEYUP && wParam == VK_INSERT)
		{
			globals::ShowMenu = !globals::ShowMenu;
		}
		if (uMsg == WM_KEYUP && wParam == VK_F12)
		{
			globals::Unload = true;
		}
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		if (!globals::ShowMenu)
			return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
		return true;
	}

	HRESULT Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (globals::Unload)
			return oPresent(pSwapChain, SyncInterval, Flags);
		static auto bMakeOnce = [&]()->bool
		{
			if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				return false;
			}
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			hWnd = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer = NULL;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			D3D11_RENDER_TARGET_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			pDevice->CreateRenderTargetView(pBackBuffer, &desc, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
			ImGui::CreateContext();
			ImGui::GetIO().IniFilename = "gui.ini";
			ImGui::GetIO().LogFilename = nullptr;
			ImGui::GetStyle().FrameRounding = 3.0f;
			ImGui::GetStyle().WindowRounding = 2.0f;
			ImGui::GetStyle().GrabRounding = 2.0f;
			ImGui::GetStyle().WindowRounding = 3.0f;
			ImGui::GetStyle().WindowTitleAlign = { 0.5f ,0.5f };
			{
				ImGuiStyle* style = &ImGui::GetStyle();
				ImVec4* colors = style->Colors;
				colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
				colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 0.40f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.67f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.22f, 0.22f, 0.22f, 0.51f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.53f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.46f, 0.46f, 0.46f, 0.31f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.32f, 0.32f, 0.32f, 0.80f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
				colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.78f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.71f, 0.71f, 0.20f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.43f, 0.43f, 0.43f, 0.95f);
				colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.40f);
				colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
				colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.80f);
				colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
				colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
				colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
				colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
				colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.36f, 0.36f, 0.36f, 0.35f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
			}
			ImGui_ImplWin32_Init(hWnd);
			ImGui_ImplDX11_Init(pDevice, pContext);
			ImGui::GetIO().Fonts->AddFontFromFileTTF("c:\\windows\\fonts\\msyh.ttc", 16, NULL, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());

			//PacketSniffer::Setup();

			return true;
		} ();
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Render();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		return oPresent(pSwapChain, SyncInterval, Flags);
	}

	HRESULT ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{
		if (globals::Unload)
			return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
		if (mainRenderTargetView)
		{
			pContext->OMSetRenderTargets(0, 0, 0);
			mainRenderTargetView->Release();
			mainRenderTargetView = NULL;
		}
		HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
		ID3D11Texture2D* pBuffer = NULL;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
		pDevice->CreateRenderTargetView(pBuffer, nullptr, &mainRenderTargetView);
		pBuffer->Release();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		D3D11_VIEWPORT vp;
		vp.Width = Width;
		vp.Height = Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pContext->RSSetViewports(1, &vp);
		return hr;
	}

	bool Setup()
	{
		if (MH_Initialize() != MH_OK)
			return false;
		static auto CreateHook = [](LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)->bool
		{
			return MH_CreateHook(pTarget, pDetour, ppOriginal) == MH_OK && MH_EnableHook(pTarget) == MH_OK;
		};
		if (!CreateHook(Direct3D.Present, Device::Present, reinterpret_cast<void**>(&Device::oPresent)))
			return false;
		if (!CreateHook(Direct3D.ResizeBuffers, Device::ResizeBuffers, reinterpret_cast<void**>(&Device::oResizeBuffers)))
			return false;
		return true;
	}
}