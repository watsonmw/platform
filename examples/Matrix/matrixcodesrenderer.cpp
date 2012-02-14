#include "stdafx.h"

#include "matrixcodesrenderer.h"
#include "matrixcodes.h"
#include "texture.h"
#include "platform/log.h"
#include "platform/imageloader.h"
#include "platform/image.h"

#include <gl/gl.h>
#include <gl/glu.h>

#include <string>
#include <vector>

using namespace std;
using namespace Platform;

class Tile
{
public:
    Tile(const Image *image, int x, int y, int w, int h);

    int w, h;             // Width and height in pixels
    float x1, y1, x2, y2; // Texuture coords
};

Tile::Tile(const Image *image, int x, int y, int w, int h)
        : w(w), h(h)
{
    x1 = x / (float)(image->getWidth() - 1);
    y1 = y / (float)(image->getHeight() - 1);
    x2 = (x + w) / (float)(image->getWidth() - 1);
    y2 = (y + h) / (float)(image->getHeight() - 1);
}

class TileRenderer
{
public:
    TileRenderer(const Image *, int screenWidth, int screenHeight);

    void renderTile(int tile, int x, int y);

    size_t getNumberOfTiles() {
        return _tiles.size();
    }

    int getGridWidth() {
        return _gridWidth;
    }
    int getGridHeight() {
        return _gridHeight;
    }

private:
    void fragmentImage(const Image *image);
    void scanLeftRight(const Image *image, int top, int bottom);

    const Image *_image;
    int _tileMaxWidth, _tileMaxHeight;
    int _screenWidth, _screenHeight;
    int _gridWidth, _gridHeight;
    std::vector<Tile *> _tiles;
};

TileRenderer::TileRenderer(const Image *image,
                           int screenWidth,
                           int screenHeight)
  : _image(image),
    _screenWidth(screenWidth),
    _screenHeight(screenHeight),
    _tileMaxWidth(0),
    _tileMaxHeight(0)
{
    fragmentImage(_image);
    size_t x = _tiles.size();
    _gridWidth = 1 + screenWidth / _tileMaxWidth;
    _gridHeight = 1 + screenHeight / _tileMaxHeight;
}

void TileRenderer::renderTile(int tile_index, int x, int y)
{
    Tile *tile = _tiles[tile_index];

    int screenX = x * _tileMaxWidth + (_tileMaxWidth - tile->w) / 2;
    int screenY = y * _tileMaxHeight + (_tileMaxHeight - tile->h) / 2;

    glTexCoord2f(tile->x1, tile->y1);
    glVertex2i(screenX, screenY);
    glTexCoord2f(tile->x1, tile->y2);
    glVertex2i(screenX, screenY + tile->h);
    glTexCoord2f(tile->x2, tile->y2);
    glVertex2i(screenX + tile->w, screenY + tile->h);
    glTexCoord2f(tile->x2, tile->y1);
    glVertex2i(screenX + tile->w, screenY);
}

void TileRenderer::fragmentImage(const Image *image)
{
    const unsigned char *data = image->getData();

    // get next clean horizontal line above non-clean line
    bool cleanabove = true;
    bool lookingfortop = true;

    int top = 0;
    int bottom = 0;
    int pixel = 0;

    for (int i = 0; i < image->getHeight(); i++) {
        bool clean = true;

        for (int j = 0; j < image->getWidth(); j++) {
            int pixel = (i * image->getWidth()) + j;

            // If blank pixel
            if (data[pixel*3] != 0 &&
                    data[(pixel*3)+1] != 0 &&
                    data[(pixel*3)+2] != 0) {
                clean = false;
                break;
            }
        }

        if (lookingfortop) {
            if (cleanabove && !clean) {
                top = i;
                lookingfortop = false;
            }
        }
        else {
            if (clean && !cleanabove) {
                bottom = i;
                lookingfortop = true;
                scanLeftRight(image, top, bottom);
            }
        }
        cleanabove = clean;
    }
}

void TileRenderer::scanLeftRight(const Image *image, int top, int bottom)
{
    const unsigned char *data = image->getData();

    // get next clean horizontal line above non-clean line
    bool clean_left = true;
    bool looking_for_left = true;
    int left = 0;
    int right = 0;

    for (int i = 0; i < image->getWidth(); i++) {
        bool clean = true;

        for (int j = top; j < bottom; j++) {
            int pixel = (j * image->getWidth()) + i;

            // If not blank pixel
            if (data[pixel*3] != 0 &&
                    data[(pixel*3)+1] != 0 &&
                    data[(pixel*3)+2] != 0) {
                clean = false;
                break;
            }
        }

        if (looking_for_left) {
            if (clean_left && !clean) {
                left = i;
                looking_for_left = false;
            }
        }
        else {
            if (clean && !clean_left) {
                right = i;
                looking_for_left = true;

                int width = right - left;
                int height = bottom - top;

                if (width > _tileMaxWidth) {
                    _tileMaxWidth = width;
                }

                if (height > _tileMaxHeight) {
                    _tileMaxHeight = height;
                }

                _tiles.push_back(new Tile(image, left, top, width, height));
            }
        }
        clean_left = clean;
    }
}

MatrixCodesRenderer::MatrixCodesRenderer(int width, int height)
  : _width(width),
    _height(height),
    _glyphSet(NULL),
    _tileRenderer(NULL),
    _matrixCodes(NULL)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

MatrixCodesRenderer::~MatrixCodesRenderer()
{
    delete _glyphSet;
    delete _tileRenderer;
    delete _matrixCodes;
}

bool MatrixCodesRenderer::loadMatrixGlyphsFromImage(const std::wstring &filePath)
{
    Image *image = ImageLoader::LoadImage(filePath);
    if (image == NULL) {
        return false;
    }

    _tileRenderer = new TileRenderer(image, _width, _height);
    _glyphSet = new Texture(image);
    _matrixCodes = new MatrixCodes(_tileRenderer->getGridWidth(),
                                   _tileRenderer->getGridHeight(),
                                   (int)_tileRenderer->getNumberOfTiles());

    return true;
}

void MatrixCodesRenderer::reloadRenderCaches()
{
    _glyphSet->updateCache();
}

void MatrixCodesRenderer::render()
{
    _matrixCodes->think();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, _width, _height, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _glyphSet->getGLTexture());

    glBegin(GL_QUADS);

    for (int i = 0; i < _tileRenderer->getGridWidth(); i++) {
        for (int j = 0; j < _tileRenderer->getGridHeight(); j++) {
            MatrixCode *code = _matrixCodes->getMatrixCode(i, j);

            int alpha = 255;

            if (code->morphGlyph != -1) {
                alpha = 255 - code->morphedFactor;

                if (code->isLeader) {
                    glColor4ub(255, 255, 255, (GLubyte)code->morphedFactor);
                }
                else {
                    glColor4ub(0, (GLubyte)code->brightness, 0, (GLubyte)code->morphedFactor);
                }
                _tileRenderer->renderTile(code->morphGlyph, i, j);
            }

            if (code->isLeader) {
                glColor4ub(255, 255, 255, alpha);
            }
            else {
                glColor4ub(0, (GLubyte)code->brightness, 0, alpha);
            }
            _tileRenderer->renderTile(code->glyph, i, j);
        }
    }
    glEnd();
    glFlush();
}