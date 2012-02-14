// Starmap.cpp: implementation of the Starmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Starmap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Starmap::Starmap()
{
    starmap1 = 1;
    starmap2 = 2;
    starmap3 = 3;

    texture = loadPcx( "data\particle", 4, 0);

    glGenTextures(1, &texture->gltexture);
    glBindTexture(GL_TEXTURE_2D, texture->gltexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texture->data);


//  Vector
    glNewList(starmap1, GL_COMPILE);
    glEndList();

}

void Starmap::Draw(void)
{
    glPushMatrix();
    glRotatef(rot1[0], 1, 0, 0);
    glRotatef(rot1[1], 0, 1, 0);
    glRotatef(rot1[2], 0, 0, 1);
    glCallList(starmap1);
    glRotatef(rot2[0], 1, 0, 0);
    glRotatef(rot2[1], 0, 1, 0);
    glRotatef(rot2[2], 0, 0, 1);
    glCallList(starmap2);
    glRotatef(rot3[0], 1, 0, 0);
    glRotatef(rot3[1], 0, 1, 0);
    glRotatef(rot3[2], 0, 0, 1);
    glCallList(starmap3);
    glPushMatrix();
}

Starmap::~Starmap()
{

}
