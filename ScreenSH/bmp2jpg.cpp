#include "pch.h"
#include "bmp2jpg.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <memory>
#include "string.h"
#include <vector>
#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

HBITMAP ResizeBmp(HBITMAP hBmpSrc, SIZE newSize)
{
	// taille actuelle 
	BITMAP bmpInfo;
	GetObject(hBmpSrc, sizeof(BITMAP), &bmpInfo);
	SIZE oldSize;
	oldSize.cx = bmpInfo.bmWidth;
	oldSize.cy = bmpInfo.bmHeight;

	// selection source ds un DC 
	HDC hdc = CreateDC((LPWSTR)"DISPLAY", NULL, NULL, NULL);
	HDC hDCSrc = CreateCompatibleDC(hdc);
	HBITMAP hOldBmpSrc = (HBITMAP)SelectObject(hDCSrc, hBmpSrc);

	// création bitmap dest et sélection ds un DC 
	HDC hDCDst = CreateCompatibleDC(hdc);
	HBITMAP hBmpDst = CreateCompatibleBitmap(hdc, newSize.cx, newSize.cy);
	HBITMAP hOldBmpDst = (HBITMAP)SelectObject(hDCDst, hBmpDst);

	// resize 
	StretchBlt(hDCDst, 0, 0, newSize.cx, newSize.cy, hDCSrc, 0, 0, oldSize.cx, oldSize.cy, SRCCOPY);

	// libération ressources 
	//destroy_DC(hdc);
	SelectObject(hDCSrc, hOldBmpSrc);
	SelectObject(hDCDst, hOldBmpDst);
	DeleteDC(hDCSrc);
	DeleteDC(hDCDst);
	ReleaseDC(NULL, hdc);

	return hBmpDst;
}
const char* SaveFile(HBITMAP a)
{

	// Initialize GDI+.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CLSID   encoderClsid;
	EncoderParameters encoderParameters;
	ULONG    quality;




	BITMAP bmpInfo;
	GetObject(a, sizeof(BITMAP), &bmpInfo);
	SIZE si;
	si.cx = bmpInfo.bmWidth;
	si.cy = bmpInfo.bmHeight;
	HBITMAP test = ResizeBmp(a, si);
	Gdiplus::Bitmap*   bitmap = Gdiplus::Bitmap::FromHBITMAP(a,NULL);
	Gdiplus::REAL dpi = 96;
	char *buffer=NULL;
	if (bitmap != 0 || bitmap != NULL)
	{
		bitmap->SetResolution(dpi, dpi);

		// Get the CLSID of the jpg encoder.
		GetEncoderClsid(L"image/jpeg", &encoderClsid);

		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
		

		quality = 100;
		encoderParameters.Parameter[0].Value = &quality;
		IStream* istream = nullptr;
		CreateStreamOnHGlobal(NULL, TRUE, &istream);



		Gdiplus::Status status = bitmap->Save(L"Mosaic2.jpg", &encoderClsid, NULL);

		delete bitmap;
		GdiplusShutdown(gdiplusToken);
	}
	return buffer;
}


