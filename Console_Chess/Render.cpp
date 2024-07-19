#include <windows.h>
#include <iostream>

#include "Server.h"

void SetFontSize(int height) {
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    float ratio = screenHeight / (float)1080;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    fontInfo.dwFontSize.X = height * ratio;
    fontInfo.dwFontSize.Y = height * ratio;
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
}

void Fullscreen() {
    SetConsoleTitleA("Chess :3");
    HWND hwnd = GetConsoleWindow();
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    SetWindowLong(hwnd, GWL_STYLE, style);
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

void NoResize() {
    HWND hwnd = GetConsoleWindow();
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    SetWindowLong(hwnd, GWL_STYLE, style);
}