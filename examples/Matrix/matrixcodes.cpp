#include "stdafx.h"

#include "matrixcodes.h"

#include <stdlib.h>
#include <time.h>

MatrixCode::MatrixCode()
{
    glyph = 0;
    morphGlyph = -1;
    morphedFactor = 0;

    brightness = 0;
    isLeader = false;

    // Seed the random number generator
    srand((unsigned int)time(NULL));
}

MatrixCodes::MatrixCodes(int width, int height, int numberOfGlyphs)
  : _width(width),
    _height(height),
    _numberOfGlyphs(numberOfGlyphs),
    _fadeSpeed(30)
{
    int size = width * height;
    _codes = new MatrixCode[size];
}

void MatrixCodes::think()
{
    const int max_new_leaders = 1;
    const int prob_line_leader = 100;
    const int max_new_morphs = 30;
    const int morph_speed = 4;
    const int max_normal_brightness = 180;

    // insert rain drops...
    int new_leaders = rand() % (max_new_leaders + 1);

    for (int i = 0; i < new_leaders; i++) {
        if ((rand() % prob_line_leader) == 3) {
            // insert a short line of drops...
            const int size = 2 + (rand() % 4);
            const int pos = rand() % (_width - size);

            for (int j = pos; j < pos + size; j++) {
                _codes[j].glyph = rand() % _numberOfGlyphs;
                _codes[j].brightness = 255;
                _codes[j].isLeader = true;
            }
        }
        else {
            // just the one
            const int pos = rand() % _width;

            _codes[pos].glyph = rand() % _numberOfGlyphs;
            _codes[pos].brightness = 255;
            _codes[pos].isLeader = true;
        }
    }

    // insert new morphing charactors
    for (int i = 0; i < max_new_morphs; i++) {
        const int pos = rand() % ((_width * _height) - 1);
        if (_codes[pos].morphGlyph == -1) {
            _codes[pos].morphGlyph = rand() % _numberOfGlyphs;
            _codes[pos].morphedFactor = 0;
        }
    }

    // Handle last row
    for (int x = 0; x < _width; x++) {
        //Filter brightness down...
        MatrixCode& code = _codes[(_height-1)*_width+x];
        if (code.brightness > _fadeSpeed) {
            if (code.isLeader) {
                code.isLeader = false;
            }
            code.brightness -= _fadeSpeed;
        }
    }

    // Handle all others
    for (int y = _height - 1; y > 0; y--) {
        for (int x = 0; x < _width; x++) {
            //Filter brightness down...
            MatrixCode& code = _codes[y*_width+x];
            MatrixCode& parent = _codes[(y-1)*_width+x];
            if (parent.brightness > _fadeSpeed) {
                if (parent.isLeader) {
                    parent.isLeader = false;
                    code.isLeader = true;
                    code.brightness = parent.brightness;
                    code.glyph = rand() % _numberOfGlyphs;
                    parent.brightness = rand() % parent.brightness;
                }
                else {
                    code.brightness += _fadeSpeed;
                }
                parent.brightness -= _fadeSpeed;
            }

            // Make sure brightness is within bounds...
            if (code.isLeader) {
                if (code.brightness > 255) {
                    code.brightness = 255;
                }
            }
            else {
                if (code.brightness > max_normal_brightness) {
                    code.brightness = max_normal_brightness;
                }
            }

            if (code.brightness < 0) {
                code.brightness = 0;
            }

            // Progress morphing...
            if (code.morphGlyph != -1) {
                code.morphedFactor += morph_speed;
                if (code.morphedFactor >= 255) {
                    code.morphedFactor = 0;
                    code.glyph = code.morphGlyph;
                    code.morphGlyph = -1;
                }
            }
        }
    }

    // Bound brigness for first line...
    for (int i = 0; i < _width; i++) {
        if (_codes[i].brightness > 255) {
            _codes[i].brightness = 255;
        }

        if (_codes[i].brightness < 0) {
            _codes[i].brightness = 0;
        }
    }
}

MatrixCode *MatrixCodes::getMatrixCode(int x, int y)
{
    return &_codes[(y*_width) + x];
}
