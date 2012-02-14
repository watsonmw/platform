#ifndef _PLATFORM_IMAGE_H_INCLUDED_
#define _PLATFORM_IMAGE_H_INCLUDED_

#include <string>

namespace Platform
{
    class Image
    {
    public:
        enum PixelFormat { NONE, GRAYSCALE, RGB, RGBA, PALETTE };

        Image();
        virtual ~Image();

        virtual bool load(const std::wstring &fileName) { return false; }

        int getHeight() const {
            return _height;
        }

        int getWidth() const {
            return _width;
        }

        int getBytesPerPixel() { return _bytesPerPixel; }

        const unsigned char *getData() const {
            return _data;
        }

        unsigned char *getWritableData() {
            return _data;
        }

        int getDataSize() const {
            return _dataSize;
        }

        PixelFormat getPixelFormat() const {
            return _pixelFormat;
        }

        void setPixelFormat(PixelFormat pixelFormat);

        void setPixelColor(int x, int y,
                           unsigned char red,
                           unsigned char green,
                           unsigned char blue,
                           unsigned char alpha = 255);

        void getPixelColor(int x, int y,
                           unsigned char &red,
                           unsigned char &green,
                           unsigned char &blue,
                           unsigned char &alpha);

        unsigned char *setImageFormat(int width,
                                      int height,
                                      PixelFormat pixelFormat);

    protected:
        static int getBytesPerPixel(PixelFormat pixelFormat);

        unsigned char *_data;
        int _width;
        int _height;
        int _dataSize;
        int _bytesPerPixel;
        PixelFormat _pixelFormat;
    };
};

#endif