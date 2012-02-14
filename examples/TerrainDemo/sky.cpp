#include "stdafx.h"
#include "sky.h"
#include "texture.h"

using namespace Platform;

Sky::Sky()
{
}

void Sky::load()
{
    _front.load(L"data/sky/skybox_front.tga", Image::RGB);
    _back.load(L"data/sky/skybox_back.tga", Image::RGB);
    _left.load(L"data/sky/skybox_left.tga", Image::RGB);
    _right.load(L"data/sky/skybox_right.tga", Image::RGB);
    _up.load(L"data/sky/skybox_up.tga", Image::RGB);
}

void Sky::draw()
{
    glEnable(GL_TEXTURE_2D);

    glDepthMask(0);
    float size = 10;

    glBindTexture(GL_TEXTURE_2D, _front.getGLTextureId());
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size, -size,  size);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( size, -size,  size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( size,  size,  size);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-size,  size,  size);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, _back.getGLTextureId());
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-size,  size, -size);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( size,  size, -size);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( size, -size, -size);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, _up.getGLTextureId());
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-size,  size, -size);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size,  size,  size);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( size,  size,  size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( size,  size, -size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, _right.getGLTextureId());
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( size,  size, -size);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( size,  size,  size);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( size, -size,  size);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, _left.getGLTextureId());
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-size, -size,  size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-size,  size,  size);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-size,  size, -size);
    glEnd();

    glBegin(GL_QUADS);
    // Bottom Face
    glTexCoord2f(1.0f, 0.40f);
    glVertex3f(-size, -size, -size);
    glTexCoord2f(0.0f, 0.40f);
    glVertex3f( size, -size, -size);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( size, -size,  size);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-size, -size,  size);
    glEnd();

    glDepthMask(1);
}

Sky::~Sky()
{
}
