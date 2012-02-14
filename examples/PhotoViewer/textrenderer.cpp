#include "stdafx.h"

#include "textrenderer.h"

#include <gdiplus.h>
using namespace Gdiplus;

#include <cstdlib>
#include <ctime>

TextRenderer::TextRenderer(const std::wstring &font)
  : font(font)
{
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Randomize std::rand() using current time
    std::srand((unsigned int)std::time(0));
}

TextRenderer::~TextRenderer()
{
    // Shutdown GDI+
    GdiplusShutdown(gdiplusToken);
}

void TextRenderer::render(HDC hdc, const std::wstring &text)
{
    Graphics graphics(hdc);

    // Find a font size suitable for this resolution
    RectF render_area_size;
    graphics.GetVisibleClipBounds(&render_area_size);
    double font_size =  render_area_size.GetBottom() / 20.0;
    if (font_size < 10.0) {
        font_size = 10.0;
    }

    // Create an antialiased font
    FontFamily font_family(font.c_str());
    Font font(&font_family, (Gdiplus::REAL)font_size, FontStyleRegular, UnitPixel);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    // Get the size of the string and find a random place to put it
    // with drawing it offscreen
    RectF string_dimensions;
    graphics.MeasureString(text.c_str(), -1, &font, render_area_size, &string_dimensions);

    int x_offset = 0;
    int max_offset_x = (int)(render_area_size.GetRight()  - string_dimensions.GetRight());
    if (max_offset_x > 0) {
        x_offset = std::rand() % max_offset_x;
    }

    int y_offset = 0;
    int max_offset_y = (int)(render_area_size.GetBottom() - string_dimensions.GetBottom());
    if (max_offset_y > 0) {
        y_offset = std::rand() % max_offset_y;
    }

    string_dimensions.Offset((Gdiplus::REAL)x_offset, (Gdiplus::REAL)y_offset);
    StringFormat string_format;
    SolidBrush white_brush(Color(255, 255, 255, 255));
    graphics.DrawString(text.c_str(), -1, &font, string_dimensions, &string_format, &white_brush);
}