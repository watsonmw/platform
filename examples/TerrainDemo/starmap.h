// Starmap.h: interface for the Starmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARMAP_H__89913B47_0DC2_11D4_86F8_00C04F32CB51__INCLUDED_)
#define AFX_STARMAP_H__89913B47_0DC2_11D4_86F8_00C04F32CB51__INCLUDED_

#include "texture.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Starmap
{
    GLuint starmap1;
    GLuint starmap2;
    GLuint starmap3;

    Vector rot1;
    Vector rot2;
    Vector rot3;

    Texture texture;
public:

    Starmap();
    virtual ~Starmap();

    void Draw(void);
};

#endif // !defined(AFX_STARMAP_H__89913B47_0DC2_11D4_86F8_00C04F32CB51__INCLUDED_)
