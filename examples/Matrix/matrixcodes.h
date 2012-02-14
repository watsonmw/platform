#ifndef _MATRIXCODES_DOT_H_INCLUDED_
#define _MATRIXCODES_DOT_H_INCLUDED_

class MatrixCode
{
public:
    MatrixCode();

    short glyph;
    short morphGlyph;
    short morphedFactor;

    short brightness;
    bool isLeader;
    bool isStreamer;
};

class MatrixCodes
{
public:
    MatrixCodes(int width, int height, int numberOfGlyphs);

    /**
     * Progress the animatation of the matrix codes by one frame
     */
    void think();

    /**
     * Returns the current matrix code at a given location
     */
    MatrixCode *getMatrixCode(int x, int y);

private:
    int _width, _height;
    int _numberOfGlyphs;
    int _fadeSpeed;
    MatrixCode *_codes;
};

#endif
