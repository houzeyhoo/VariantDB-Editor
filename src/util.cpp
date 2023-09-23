#include "util.hpp"
#include "../res/resource.h"
#include <algorithm>
#include <codecvt>
#include <iostream>

namespace D3DState
{
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;

	void CreateRenderTarget()
	{
		ID3D11Texture2D* buf = nullptr;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&buf));
		device->CreateRenderTargetView(buf, nullptr, &renderTargetView);
		buf->Release();
	}

	void DestroyRenderTarget()
	{
		if (renderTargetView)
		{
			renderTargetView->Release();
			renderTargetView = nullptr;
		}
	}

	bool CreateDevice(HWND window)
	{
		DXGI_SWAP_CHAIN_DESC sc_desc = {};
		sc_desc.BufferCount = 2;
		sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sc_desc.OutputWindow = window;
		sc_desc.SampleDesc.Count = 1;
		sc_desc.Windowed = TRUE;

		HRESULT res = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
			&sc_desc, &swapChain, &device, nullptr, &deviceContext
		);
		if (res == DXGI_ERROR_UNSUPPORTED)
		{
			HRESULT res = D3D11CreateDeviceAndSwapChain(
				nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
				&sc_desc, &swapChain, &device, nullptr, &deviceContext
			);
		}
		if (res != S_OK)
			return false;
		CreateRenderTarget();
		/* Disable ALT + Enter */
		IDXGIFactory1* factory = nullptr;
		if (SUCCEEDED(swapChain->GetParent(__uuidof(IDXGIFactory1), (void**)&factory)))
		{
			factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);
			factory->Release();
		}
		return true;
	}

	void DestroyDevice()
	{
		DestroyRenderTarget();
		if (swapChain)
		{
			swapChain->Release();
			swapChain = nullptr;
		}
		if (deviceContext)
		{
			deviceContext->Release();
			deviceContext = nullptr;
		}
		if (device)
		{
			device->Release();
			device = nullptr;
		}
	}

	void HandleResize(UINT width, UINT height)
	{
		DestroyRenderTarget();
		swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		CreateRenderTarget();
	}

	void PrepareForPresenting()
	{
		deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
		float bg[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		deviceContext->ClearRenderTargetView(renderTargetView, bg);
	}

	void Present(UINT syncInterval)
	{
		swapChain->Present(syncInterval, 0);
	}
};

HWND RegisterAndCreateWindow(std::wstring wndName, WNDPROC wndProc, UINT width, UINT height)
{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = wndProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = (wndName + L"_WndClass").data();
	wc.hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassExW(&wc);
	return CreateWindowExW(
		0, wc.lpszClassName, wndName.data(), WS_OVERLAPPEDWINDOW, 100, 100,
		width, height, nullptr, nullptr, wc.hInstance, nullptr
	);
}

void UnregisterAndDestroyWindow(HWND wnd)
{
	std::wstring className(256, L'\0');
	GetClassNameW(wnd, &className[0], 256);
	DestroyWindow(wnd);
	UnregisterClassW(className.data(), nullptr);
}

std::optional<std::wstring> ShowOpenFileDialog(HWND parent)
{
	std::wstring path(MAX_PATH, L'\0');
	OPENFILENAMEW ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = &path[0];
	ofn.nMaxFile = MAX_PATH;
	if (!GetOpenFileNameW(&ofn))
		return {};
	path.erase(std::find(path.begin(), path.end(), '\0'), path.end());
	return path;
}

std::optional<std::wstring> ShowSaveFileDialog(HWND parent)
{
	std::wstring path(MAX_PATH, L'\0');
	OPENFILENAMEW ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = &path[0];
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	if (!GetSaveFileNameW(&ofn))
		return {};
	path.erase(std::find(path.begin(), path.end(), '\0'), path.end());
	return path;
}

std::string WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

bool IsStringPrintable(const std::string& str)
{
	if (str.empty())
		return true;
	return std::any_of(str.begin(), str.end(),
		[](unsigned char c) { return std::isprint(c); });
}

std::string GetLowercaseString(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return str;
}

void ErrorMessageBox(const std::string& text)
{
	MessageBoxA(nullptr, text.data(), "VariantDB Editor", MB_OK | MB_ICONERROR);
}