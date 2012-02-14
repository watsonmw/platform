#ifndef _WIN32_DOT_H_INCLUDED
#define _WIN32_DOT_H_INCLUDED

#include <string>

/**
 * Namespace containing wrappers for win32 functions.
 */
namespace Win32
{
    std::wstring GetMyPicturesPath();
    std::wstring GetMyInternetCachePath();
};

#endif