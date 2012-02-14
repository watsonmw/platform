#include "stdafx.h"

#include "platform/pcximage.h"
#include "platform/log.h"

namespace Platform
{
    PCXImage::PCXImage()
      : Image(),
        _palette(0)
    {
    }

    PCXImage::~PCXImage()
    {
        delete _palette; _palette = 0;
    }

    bool PCXImage::load(const std::wstring &filePath)
    {
        Log::printf(L"Loading \'%s\'...", filePath.c_str());

        FILE *fp = _wfopen(filePath.c_str(), L"rb");

        if (fp == NULL) {
            Log::printf(L"File \'%s\' not found.", filePath.c_str());
            return false;
        }

        unsigned char c;

        if ((c = getc(fp)) != 10) {
            Log::printf(L"Error: %s not a pcx file.", filePath.c_str());
            fclose(fp);
            return false;
        }

        if ((c = getc(fp)) != 5) {
            Log::printf(L"Error: Incorrect pcx verson : %s ", filePath.c_str());
            fclose(fp);
            return false;
        }

        if ((c = getc(fp)) != 1) {
            Log::printf(L"Error: Dunno how to read this pcx file : %s", filePath.c_str());
            fclose(fp);
            return false;
        }

        if ((c = getc(fp)) != 8) {
            Log::printf(L"Error: Palette size not supported : %s.", filePath.c_str());
            fclose(fp);
            return false;
        }

        int sx, sy, ex, ey;

        sx = fgetc(fp);
        sx |= fgetc(fp) << 8;

        sy = fgetc(fp);
        sy |= fgetc(fp) << 8;

        ex = fgetc(fp);
        ex |= fgetc(fp) << 8;

        ey = fgetc(fp);
        ey |= fgetc(fp) << 8;

        _width = ex - sx + 1;
        _height = ey - sy + 1;

        Log::printf(L"PCX Image: %d*%d", _width, _height);

        _data = new unsigned char[_width * _height];

        if (_data == NULL) {
            Log::print(L"Error allocating memory! Is the pcx file correct???");
            return false;
        }

        _bytesPerPixel = 1;
        _pixelFormat = PALETTE;
        _palette = new unsigned char[256 * 3];

        fseek(fp, 128, SEEK_SET);

        int i = 0;
        while (i < (_width * _height)) {
            c = getc(fp);

            if (c > 0xbf) {
                int repeat = 0x3f & c;
                c = getc(fp);

                for (int j = 0; j < repeat; j++) {
                    _data[i++] = c;
                }
            }
            else {
                _data[i++] = c;
            }
            fflush(stdout);
        }

        fseek(fp, -769, SEEK_END);

        if ((c = getc(fp)) != 12) {
            fclose(fp);
            return false;
        }

        for (i = 0; i < 768; i++) {
            c = getc(fp);
            _palette[i] = c;
        }

        fclose(fp);

        return true;
    }

    Image* PCXImage::convert(PixelFormat pixelFormat)
    {
        Image* image = new Image();
        image->setImageFormat(getWidth(), getHeight(), pixelFormat);

        for (int i = 0; i < getWidth() * getHeight(); i++) {
            unsigned char red, green, blue, alpha;
            red   = _palette[(_data[i] * 3)];
            blue  = _palette[(_data[i] * 3) + 1];
            green = _palette[(_data[i] * 3) + 2];

            if (pixelFormat == RGBA && _data[i] == 0) {
                alpha = 0;
            }
            else {
                alpha = 255;
            }

            image->setPixelColor(i, 0, red, green, blue, alpha);
        }
        return image;
    }

    void PCXImage::getPaletteColor(int i,
                                   unsigned char &red,
                                   unsigned char &green,
                                   unsigned char &blue)
    {
        red = _palette[i];
        green = _palette[i+1];
        blue = _palette[i+2];
    }

    void PCXImage::getPixelColor(int x, int y,
                                 unsigned char &red,
                                 unsigned char &green,
                                 unsigned char &blue,
                                 unsigned char &alpha)
    {
        int pixelIndex = ((y * _width) + x) * _bytesPerPixel;
        int colorIndex = _data[pixelIndex];
        red = _palette[colorIndex];
        green = _palette[colorIndex+1];
        blue = _palette[colorIndex+2];
        alpha = 255;
    }

    int PCXImage::getPaletteIndex(int x, int y)
    {
        if (x < 0 || x >= _width || y < 0 || y >= _height) {
            return -1;
        }

        int pixelIndex = ((y * _width) + x) * _bytesPerPixel;
        return _data[pixelIndex];
    }
};
