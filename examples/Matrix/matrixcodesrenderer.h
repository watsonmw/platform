#ifndef _MATRIXCODESRENDERER_DOT_H_INCLUDED_
#define _MATRIXCODESRENDERER_DOT_H_INCLUDED_

#include <string>

class Texture;
class TileRenderer;
class MatrixCodes;

class MatrixCodesRenderer
{
public:
    MatrixCodesRenderer(int width, int height);
    ~MatrixCodesRenderer();

    /**
     * Reload the render caches into the current GL context
     */
    bool loadMatrixGlyphsFromImage(const std::wstring &filePath);

    /**
     * Reload the render caches into the current GL context
     */
    void reloadRenderCaches();

    /**
     * Render the next frame
     */
    void render();

private:
    int _width, _height;
    Texture *_glyphSet;
    TileRenderer *_tileRenderer;
    MatrixCodes *_matrixCodes;
};

#endif
