#ifndef FILE_H
#define FILE_H

#if defined(MYLIBRARY)
#define LIBRARY_EXPORT __declspec(dllexport)
#else
#define LIBRARY_EXPORT
#endif
#include <Windows.h>
extern "C" int LIBRARY_EXPORT Val_One(HWND hwnd);
extern "C" int LIBRARY_EXPORT Val_Two();

#endif // FILE_H