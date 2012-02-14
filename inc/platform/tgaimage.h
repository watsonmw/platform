#ifndef _PLATFORM_TGAIMAGE_H_INCLUDED_
#define _PLATFORM_TGAIMAGE_H_INCLUDED_

#include "platform/image.h"

namespace Platform
{
    class TGAImage : public Image
    {
    public:
        TGAImage();
        virtual ~TGAImage();

        virtual bool load(const std::wstring &filePath);
    };
};

#endif