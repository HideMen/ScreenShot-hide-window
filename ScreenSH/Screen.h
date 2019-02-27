#include <Windows.h>
#include "pch.h"
#include <vector>
#pragma once

const char* MakeScreenShot(HWND hWnd, HDC hDC_res, DWORD* W, DWORD* H);
const char* ScreenShot(HWND hwnd);