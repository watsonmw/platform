#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stdafx.h"
#include "log.h"
#include "texture.h"
#include "pcx.h"

#include "platform/log.h"
#include "platform/imageloader.h"

using namespace Platform;

Texture::Texture()
  : _image(NULL),
    _glTextureId(0)
{
}

Texture::~Texture()
{
    delete _image; _image = NULL;
}

bool Texture::load(const std::wstring& filePath, Platform::Image::PixelFormat pixelFormat)
{
    _image = ImageLoader::LoadImage(filePath, pixelFormat);

    if (_image == NULL) {
        Log::printf(L"Failed to load %s", filePath);
        return false;
    }

    glGenTextures(1, &_glTextureId);
    glBindTexture(GL_TEXTURE_2D, _glTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    GLenum format = GL_RGB;
    switch(pixelFormat) {
        case Platform::Image::RGB:
            format = GL_RGB;
            break;
        case Platform::Image::RGBA:
            format = GL_RGBA;
            break;
        case Platform::Image::GRAYSCALE:
            format = GL_LUMINANCE;
            break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, _image->getBytesPerPixel(),
        _image->getWidth(), _image->getHeight(), 0,
        format, GL_UNSIGNED_BYTE, _image->getData());
    Log::logOpenGLError();

    return true;
}
