#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define NOMINMAX
#include <d3d11.h>
#include <algorithm>
#include <string>
#include <optional>

// D3D11 boilerplate code
namespace D3DState
{
    extern ID3D11Device* device;
    extern ID3D11DeviceContext* deviceContext;
    extern IDXGISwapChain* swapChain;
    extern ID3D11RenderTargetView* renderTargetView;

    void CreateRenderTarget();
    void DestroyRenderTarget();
    bool CreateDevice(HWND window);
    void DestroyDevice();
    void HandleResize(UINT width, UINT height);
    void PrepareForPresenting();
    void Present(UINT syncInterval);
};

HWND RegisterAndCreateWindow(std::wstring wndName, WNDPROC wndProc, UINT width, UINT height);
void UnregisterAndDestroyWindow(HWND wnd);

std::optional<std::wstring> ShowOpenFileDialog(HWND parent);
std::optional<std::wstring> ShowSaveFileDialog(HWND parent);

std::string WStringToString(const std::wstring& wstr);
bool IsStringPrintable(const std::string& str);
std::string GetLowercaseString(std::string str);

void ErrorMessageBox(const std::string& text);