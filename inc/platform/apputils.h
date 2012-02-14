#ifndef _PLATFORM_APPUTILS_H_INCLUDED_
#define _PLATFORM_APPUTILS_H_INCLUDED_

#include <string>

namespace Platform
{
    namespace AppUtils
    {
        std::wstring getBasePath();
        void setBasePath(const std::wstring &basePath);

        std::wstring removeWhiteSpace(const std::wstring &str);
    };
};
#endif