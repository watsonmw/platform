#ifndef _TEXTURE_H_INCLUDED_
#define _TEXTURE_H_INCLUDED_

#include <string>

#include "platform/image.h"

class Texture
{
public:
    Texture(Platform::Image *image);
    virtual ~Texture();

    void updateCache();
    unsigned int getGLTexture() {
        return gltexture;
    };

private:
    Platform::Image *image;
    unsigned int gltexture;
};

#endif