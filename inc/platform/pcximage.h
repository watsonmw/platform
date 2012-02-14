#ifndef _PLATFORM_PCXIMAGE_H_INCLUDED_
#define _PLATFORM_PCXIMAGE_H_INCLUDED_

#include "platform/image.h"

namespace Platform
{
    class PCXImage : public Image
    {
    public:
        PCXImage();
        virtual ~PCXImage();

        virtual bool load(const std::wstring &filePath);

        int getPaletteIndex(int x, int y);

        void getPaletteColor(int i,
                             unsigned char &red,
                             unsigned char &green,
                             unsigned char &blue);

        void getPixelColor(int x, int y,
                           unsigned char &red,
                           unsigned char &green,
                           unsigned char &blue,
                           unsigned char &alpha);

        Image* convert(PixelFormat pixelFormat);

    private:
        unsigned char *_palette;
    };
};

#endif