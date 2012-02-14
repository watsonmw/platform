#ifndef _PLATFORM_WIN32_DOT_H_INCLUDED
#define _PLATFORM_WIN32_DOT_H_INCLUDED

#include <string>

/**
 * Namespace containing wrappers for win32 functions.
 */
namespace Platform
{
    namespace Win32
    {
        std::wstring getMyPicturesPath();
        std::wstring getMyInternetCachePath();
        std::wstring getExecutableDir();
        int getKeyState(char x);
    };
};
#endif