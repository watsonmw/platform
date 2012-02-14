#include "stdafx.h"

#include "platform/image.h"

namespace Platform
{
    Image::Image()
      : _data(NULL),
        _dataSize(0),
        _height(0),
        _width(0),
        _pixelFormat(NONE)
    {
    }

    Image::~Image()
    {
        delete _data; _data = 0;
    }

    void Image::setPixelFormat(PixelFormat pixelFormat)
    {
        _bytesPerPixel = getBytesPerPixel(pixelFormat);
        _pixelFormat = pixelFormat;
    }

    int Image::getBytesPerPixel(PixelFormat pixelFormat)
    {
        switch (pixelFormat) {
        case GRAYSCALE:
            return 1;
        case RGB:
            return 3;
        case RGBA:
            return 4;
        }

        return 0;
    }

    unsigned char* Image::setImageFormat(int width, int height, PixelFormat pixelFormat)
    {
        delete _data; _data = 0;

        _width = width;
        _height = height;

        setPixelFormat(pixelFormat);

        //
        // Setup the raw pixel data buffer
        //
        _dataSize = width * height * _bytesPerPixel;
        _data = new unsigned char[_dataSize];

        return _data;
    }

    void Image::setPixelColor(int x, int y,
                              unsigned char red,
                              unsigned char green,
                              unsigned char blue,
                              unsigned char alpha)
    {
        int pixelIndex = ((y * _width) + x) * _bytesPerPixel;
        if (_pixelFormat == GRAYSCALE) {
            _data[pixelIndex] = ((int)red + (int)green + (int)blue) / 3;
        } else {
            _data[pixelIndex] = red;
            _data[pixelIndex+1] = green;
            _data[pixelIndex+2] = blue;

            if (_pixelFormat == RGBA) {
                _data[pixelIndex+3] = alpha;
            }
        }
    }

    void Image::getPixelColor(int x, int y,
                              unsigned char &red,
                              unsigned char &green,
                              unsigned char &blue,
                              unsigned char &alpha)
    {
        int pixelIndex = ((y * _width) + x) * _bytesPerPixel;
        if (_pixelFormat == GRAYSCALE) {
            red = green = blue = alpha = _data[pixelIndex];
        } else {
            red = _data[pixelIndex];
            green = _data[pixelIndex+1];
            blue = _data[pixelIndex+2];

            if (_pixelFormat == RGBA) {
                alpha = _data[pixelIndex+3];
		    } else {
                alpha = 255;
		    }
        }
    }
};
