#include "stdafx.h"

#include "win32.h"

#include <shlobj.h>

static std::wstring Win32SHGetSpecialFolderPath(int specialFolder)
{
    TCHAR buffer[MAX_PATH+1];
    if (SHGetSpecialFolderPath(NULL,
                               buffer,
                               specialFolder,
                               true))
        return buffer;

    return L"";
}


std::wstring Win32::GetMyPicturesPath()
{
    return Win32SHGetSpecialFolderPath(CSIDL_MYPICTURES);
}


std::wstring Win32::GetMyInternetCachePath()
{
    return Win32SHGetSpecialFolderPath(CSIDL_INTERNET_CACHE);
}

