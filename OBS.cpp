#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <fstream>
#include <string>
#include <filesystem>
#include "Prototype.h"
namespace fs = std::filesystem;

std::string statusText = "INITIALIZING G|72...";
HBITMAP hSplashImage = NULL;
bool isPanic = false;

void LogError(const std::string& msg) {
    std::ofstream log("almondfaillog.txt", std::ios::app);
    log << "[FATAL] " << msg << "\n";
}

HBITMAP CargarBMP(const std::string& path) {
    return (HBITMAP)LoadImageA(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void RenderizarPanic(HWND hwnd, const std::string& msg) {
    LogError(msg);
    isPanic = true;
    hSplashImage = CargarBMP("ASSETS/panic.BMP");
    statusText = "SYSTEM FAILURE: " + msg;
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    MessageBoxA(hwnd, msg.c_str(), "G|72 CRITICAL ERROR", MB_OK | MB_ICONERROR);
    exit(1);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (hSplashImage) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                SelectObject(hdcMem, hSplashImage);
                BitBlt(hdc, 0, 0, 420, 262, hdcMem, 0, 0, SRCCOPY);
                DeleteDC(hdcMem);
            }
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, isPanic ? RGB(255, 0, 0) : RGB(200, 200, 200));
            TextOutA(hdc, 10, 220, statusText.c_str(), (int)statusText.length());
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WindowProc, 0, 0, hInstance, 
                     (HICON)LoadImage(NULL, "RibbonICON.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE), 
                     NULL, NULL, NULL, "G72_Class", 
                     (HICON)LoadImage(NULL, "RibbonICON.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE)};
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, "G72_Class", "G|72 OBS", WS_POPUP | WS_VISIBLE, 
                               (GetSystemMetrics(SM_CXSCREEN)-420)/2, (GetSystemMetrics(SM_CYSCREEN)-262)/2, 
                               420, 262, NULL, NULL, hInstance, NULL);

    hSplashImage = CargarBMP("ASSETS/boot.BMP");
    
    if (!fs::exists("ASSETS")) RenderizarPanic(hwnd, "ASSETS MISSING");
    
    statusText = "DIRECT3D 2D BLITTER INITIALIZATION... [ OK ]";
    InvalidateRect(hwnd, NULL, TRUE); UpdateWindow(hwnd); Sleep(800);

    statusText = "VERIFYING PSYCHPY RUNTIME... [ OK ]";
    InvalidateRect(hwnd, NULL, TRUE); UpdateWindow(hwnd);
    if (!fs::exists("PsychPY/Lib/Plat-Win/Socket.py")) RenderizarPanic(hwnd, "RUNTIME ERROR");
    
    statusText = "SYSTEM READY. STARTING...";
    InvalidateRect(hwnd, NULL, TRUE); UpdateWindow(hwnd); Sleep(1000);

    DestroyWindow(hwnd);
    
    int gameResult = StartGameLoop(hInstance, nCmdShow);

    return gameResult;
}
