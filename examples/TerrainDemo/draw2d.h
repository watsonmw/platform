#ifndef DRAW2D_DOT_H
#define DRAW2D_DOT_H

#include "texture.h"
#include <map>

class SpriteRenderer {
public:
    ~SpriteRenderer();

    bool load();
    bool load(const std::string& name);
    Texture *getSprite(const std::string& name);
    void drawSprite(const std::string& name, int x, int y, int xe, int ye);

private:
    typedef std::map<const std::string, Texture *> Sprites;
    Sprites _sprites;
};

class TextRenderer {
public:
    TextRenderer();

    bool load(const std::wstring& file);

    void drawString(char *c, int x, int y);
    void drawPrintf(int x, int y, char *p, ...);
    void drawStringSlow(int x, int y, char *, int speed);
    void updateString();

private:
    void drawChar(int c, int x, int y);

    Texture _fontTexture;
    int _lastDrawTime;
    int _stringSpeed;
    char _stringBuffer[1024];
    int _stringToUpdate;
    unsigned int _stringIndex;
    int _stringX, _stringY;
};

#endif