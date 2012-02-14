#ifndef SKY_DOT_H
#define SKY_DOT_H

#include "texture.h"

class Sky
{
public:
    Sky();
    ~Sky();

    void load();
    void cleanup();
    void draw();

private:
    Texture _front;
    Texture _left;
    Texture _right;
    Texture _back;
    Texture _up;
};

#endif