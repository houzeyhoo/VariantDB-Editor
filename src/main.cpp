#include "editor.hpp"
#include "util.hpp"
#include "variant.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		D3DState::HandleResize(0, 0);
		return 0;
	case WM_SYSCOMMAND:
		return (wParam & 0xfff0) == SC_KEYMENU ? 0 : DefWindowProcW(hWnd, msg, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
}

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	// Setup window and D3D11
	HWND window = RegisterAndCreateWindow(L"VariantDB Editor", WndProc, 720, 480);
	if (!D3DState::CreateDevice(window))
	{
		D3DState::DestroyDevice();
		UnregisterAndDestroyWindow(window);
		return 1;
	}
	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
	// Setup ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(D3DState::device, D3DState::deviceContext);
	ImGui::StyleColorsDark();

	// Main loop
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// If not capturing keyboard or window is not focused, wait for messages
		if (!io.WantCaptureKeyboard || GetFocus() != window)
			MsgWaitForMultipleObjectsEx(0, nullptr, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
		while (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			if (msg.message == WM_QUIT)
				break;
		}
		// Setup frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// GUI
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);
		static VariantDBEditor editor(window);
		editor.Show(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
		// Render frame
		ImGui::Render();
		D3DState::PrepareForPresenting();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		D3DState::Present(1);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	D3DState::DestroyDevice();
	UnregisterAndDestroyWindow(window);
	return 0;
}