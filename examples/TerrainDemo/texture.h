#ifndef _TEXTURE_DOT_H
#define _TEXTURE_DOT_H

#include "platform/image.h"

class Texture {
public:
    Texture();
    ~Texture();

    GLuint getGLTextureId() { return _glTextureId; }

    bool load(const std::wstring& filePath, Platform::Image::PixelFormat pixelFormat);

private:
    Platform::Image* _image;
    GLuint _glTextureId;
};

#endif
