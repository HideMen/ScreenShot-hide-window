#include "pch.h"
#include "File.h"
#include "Screen.h"
#include <Windows.h>

int Val_One(HWND hwnd)
{
	ScreenShot(hwnd);
	return 0;
}

int Val_Two()
{
	MessageBox(NULL, L"Helou!", L"Ok", 0);
	return 1;
}

#include <windows.h>

extern "C" __declspec(dllexport) void Message();

void Message()
{
	ScreenShot(NULL);
}