#include "stdafx.h"

#include "platform/win32.h"

#include <shlobj.h>

using namespace std;

static wstring Win32SHGetSpecialFolderPath(int specialFolder)
{
    TCHAR buffer[MAX_PATH+1];
    if (SHGetSpecialFolderPath(NULL,
                               buffer,
                               specialFolder,
                               true)) {
        return buffer;
    }

    return L"";
}

namespace Platform
{
    wstring Win32::getMyPicturesPath()
    {
        return Win32SHGetSpecialFolderPath(CSIDL_MYPICTURES);
    }

    wstring Win32::getMyInternetCachePath()
    {
        return Win32SHGetSpecialFolderPath(CSIDL_INTERNET_CACHE);
    }

    wstring Win32::getExecutableDir()
    {
        wstring exec_dir;
        HINSTANCE module = GetModuleHandle(NULL);

        TCHAR c_file_name[MAX_PATH+1];
        if (GetModuleFileName(module, c_file_name, MAX_PATH)) {
            wstring file_name = c_file_name;
            size_t pos = file_name.find_last_of(L"\\");
            if (pos != -1) {
                exec_dir = file_name.substr(0, pos);
            }
        }

        return exec_dir;
    }


    int Win32::getKeyState(char x)
    {
        SHORT pressed = GetAsyncKeyState((int) x);
        if (pressed) {
            int just_pressed = 1 & pressed;
            if (just_pressed) {
                return 2;
            }
            else {
                return 1;
            }
        }
        else {
            return 0;
        }
    }
};
