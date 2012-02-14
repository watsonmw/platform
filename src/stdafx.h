// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

// Needs Windows 2tow or later to compile
#define _WIN32_WINDOWS 0x0410
#define WINVER 0x0500

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
