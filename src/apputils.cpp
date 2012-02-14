#include "stdafx.h"

#include "platform/apputils.h"
#include "platform/settings.h"
#include "platform/log.h"
#include "platform/win32.h"

using namespace std;

static wstring sBasePath = L"";

namespace Platform
{
    wstring AppUtils::getBasePath()
    {
        if (sBasePath.empty()) {
            sBasePath = Win32::getExecutableDir() + L"\\Data\\";
        }

        return sBasePath;
    }

    void AppUtils::setBasePath(const wstring &basePath)
    {
        sBasePath = basePath;
    }

    wstring AppUtils::removeWhiteSpace(const wstring &str)
    {
        wstring strippedString;

        for (wstring::const_iterator i = str.begin(); i < str.end(); i++) {
            if (*i != ' ' && *i != '\t' &&
                *i != '\n' && *i != '\r') {
                strippedString += *i;
            }
        }

        return strippedString;
    }
};
