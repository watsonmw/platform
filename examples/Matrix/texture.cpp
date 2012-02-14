#include "stdafx.h"

#include "texture.h"

#include <windows.h>
#include <gl/gl.h>

#include "platform/log.h"

using namespace std;
using namespace Platform;

Texture::Texture(Image *image)
  : image(image), gltexture(0)
{
    updateCache();
}

void Texture::updateCache()
{
    glGenTextures(1, &gltexture);
    glBindTexture(GL_TEXTURE_2D, gltexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    switch (image->getPixelFormat()) {
    case Image::RGBA:
        glTexImage2D(GL_TEXTURE_2D, 0, 4, image->getWidth(), image->getHeight(), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image->getData());
        break;

    case Image::RGB:
        glTexImage2D(GL_TEXTURE_2D, 0, 3, image->getWidth(), image->getHeight(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image->getData());
        break;
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &gltexture);
    delete image;
}
