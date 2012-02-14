#include "stdafx.h"
#include "stringutils.h"
#include "draw2d.h"
#include "texture.h"
#include "platform/log.h"

using namespace Platform;

TextRenderer::TextRenderer()
   : _lastDrawTime(0),
     _stringSpeed(0),
     _stringToUpdate(0),
     _stringIndex(0),
     _stringX(0),
     _stringY(0)
{
    memset(_stringBuffer, 0, sizeof(_stringBuffer));
}

bool TextRenderer::load(const std::wstring& file)
{
    if (!_fontTexture.load(file, Platform::Image::RGBA)) {
        Log::printf(L"Failed to load %s: charactor file.", file.c_str());
        return false;
    }
    return true;
}

void TextRenderer::drawChar(int c, int x, int y)
{
    if (c == 32) {
        return;
    }

    if (y <= -8) {
        return;
    }

    int row = (int)c >> 4;
    int col = (int)c & 15;

    float frow = row * 0.0625f;
    float fcol = col * 0.0625f;
    float size = 0.0625f;

    glBegin(GL_QUADS);
    glTexCoord2f(fcol, frow);
    glVertex2i(x, y);

    glTexCoord2f(fcol + size, frow);
    glVertex2i(x + 8, y);

    glTexCoord2f(fcol + size, frow + size);
    glVertex2i(x + 8, y + 8);

    glTexCoord2f(fcol, frow + size);
    glVertex2i(x, y + 8);
    glEnd();
}

void TextRenderer::drawString(char *c, int x, int y)
{
    if (c == NULL) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, _fontTexture.getGLTextureId());
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0, 1.0, 1.0);

    while (*c != 0) {
        drawChar(*c, x, y);
        x += 8;
        c++;
    }
}

void TextRenderer::drawPrintf(int x, int y, char *p, ...)
{
    char buffer[1024];
    va_list args;

    va_start(args, p);
    vsprintf(buffer, p, args);
    va_end(args);

    drawString(buffer, x, y);

    Log::logOpenGLError();
}

void TextRenderer::drawStringSlow(int x, int y, char *p, int speed)
{
    strcpyn(_stringBuffer, p, 1024);
    _stringX = x;
    _stringY = y;
    _stringIndex = 0;
    _lastDrawTime = timeGetTime();
    _stringToUpdate = 1;
    _stringSpeed = speed;
}

void TextRenderer::updateString()
{
    int time;
    char buff[1024];

    if (_stringToUpdate) {
        time = timeGetTime() - _lastDrawTime;

        if (_stringSpeed == 0 ) {
            drawString(_stringBuffer, _stringX, _stringY);
            _stringToUpdate = 0;
        }
        _stringIndex += time / _stringIndex;

        if (_stringIndex >= strlen(_stringBuffer)) {
            _stringIndex = strlen(_stringBuffer);
            _stringToUpdate = 0;
        }

        strcpyn(buff, _stringBuffer, _stringIndex);
        drawString(buff, _stringX, _stringY);

        if (time > _stringSpeed) {
            _lastDrawTime = timeGetTime();
        }
    }
}

SpriteRenderer::~SpriteRenderer() {
    for (Sprites::iterator i = _sprites.begin(); i != _sprites.end(); ++i) {
        Texture* texture = (*i).second;
        delete texture; texture = 0;
    }
    _sprites.clear();
}

bool SpriteRenderer::load()
{
    return load("data/sprites.cfg");
}

bool SpriteRenderer::load(const std::string& name)
{
    int i;
    char textureName[256];
    char texturePath[256];

    FILE* fp = fopen(name.c_str(), "r");

    if (fp == NULL) {
        Log::printf(L"Failed to open sprites file %s.", name.c_str());
        return 0;
    }

    Log::printf(L"Parsing %s...", name.c_str());

    int numSprites = 0;
    fscanf(fp, "%d", &numSprites);

    for (i = 0; i < numSprites; i++) {
        fscanf(fp, "%s %s", textureName, texturePath);

        Texture* sprite = new Texture();
        if (sprite->load(/* TODO: charactor conversion functions texturePath*/L"", Image::RGBA)) {
            delete sprite;
            Log::printf(L"Failed to load %s: texture file.", texturePath);
            break;
        }
        _sprites[textureName] = sprite;
    }
    fclose(fp);

    return 1;
}

Texture *SpriteRenderer::getSprite(const std::string& name)
{
    Texture* sprite = _sprites.at(name);
    if (!sprite) {
        Log::printf(L"Sprite \"%s\" not found.", name.c_str());
    }
    return sprite;
}

void SpriteRenderer::drawSprite(const std::string& name, int x, int y, int xe, int ye)
{
    Texture* texture = getSprite(name);
    if (texture == NULL) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, texture->getGLTextureId());
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);
    glVertex2i(x, y);

    glTexCoord2f(1, 1);
    glVertex2i(xe, y);

    glTexCoord2f(1, 0);
    glVertex2i(xe, ye);

    glTexCoord2f(0, 0);
    glVertex2i(x, ye);

    glEnd();
}
