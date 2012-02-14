#ifndef _TEXTRENDERER_DOT_H_INCLUDED
#define _TEXTRENDERER_DOT_H_INCLUDED

#include <windows.h>
#include <string>

/**
 * Class for draw anitaliased text
 */
class TextRenderer
{
public:
    TextRenderer(const std::wstring &font);
    virtual ~TextRenderer();

    /**
     * Render the given string at a random location
     * on the screen.  Wrap the string if its too long.
     * Use a font size porportional to the hdc size.
     */
    void render(HDC hdc, const std::wstring &text);

private:
    ULONG_PTR gdiplusToken;
    std::wstring font;
};

#endif
