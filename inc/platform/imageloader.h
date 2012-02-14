#ifndef _PLATFORM_PLATFORM_IMAGELOADER_DOT_H_
#define _PLATFORM_PLATFORM_IMAGELOADER_DOT_H_

#include <string>
#include "platform/image.h"

namespace Platform
{
    class ImageLoader
    {
    public:
        static Image *LoadImage(const std::wstring &image,
                                Image::PixelFormat pixelFormat = Image::RGB);
    };
};

#endif
