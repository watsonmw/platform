#include "stdafx.h"

#include <stdio.h>

#include "platform/tgaimage.h"
#include "platform/log.h"

namespace Platform
{
    TGAImage::TGAImage()
    {
    }

    TGAImage::~TGAImage()
    {
    }

    bool TGAImage::load(const std::wstring &filePath)
    {
        Log::printf(L"Loading \'%s\'...", filePath.c_str());
        FILE *fp = _wfopen(filePath.c_str(), L"rb");

        if (fp == NULL) {
            return false;
        }

        unsigned char *header = new unsigned char[18];

        size_t bytesRead = fread(header, 1, 18, fp);

        if (bytesRead != 18) {
            return false;
        }

        int bitsPerPixel = header[16];

        PixelFormat filePixelFormat;
        if (bitsPerPixel == 24) {
            filePixelFormat = Image::RGB;
        }
        else if (bitsPerPixel == 32) {
            filePixelFormat = Image::RGBA;
        }
        else {
            Log::print(L"Image must be in 24 or 32bit uncompressed TGA format.");
            return false;
        }

        int width = header[12] + (header[13] * 256);
        int height = header[14] + (header[15] * 256);

        delete [] header; header = 0;

        Log::printf(L"TGA Image: %d*%d*%d", width, height, bitsPerPixel);

        Image rawImage;
        rawImage.setImageFormat(width, height, filePixelFormat);

        setImageFormat(width, height, _pixelFormat);

        fseek(fp, 18, SEEK_SET);
        fread(rawImage.getWritableData(), 1, rawImage.getDataSize(), fp);
        fclose(fp);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                unsigned char blue, green, red, alpha;
                rawImage.getPixelColor(j, i, blue, green, red, alpha);
                setPixelColor(j, height - i - 1, red, green, blue, alpha);
            }
        }

        return true;
    }
}
