#ifndef TERRAINDEMO_DOT_H
#define TERRAINDEMO_DOT_H

#include "platform/application.h"

class Terrain;
class Viewer;
class SpriteRenderer;
class TextRenderer;

class TerrainDemo : public Platform::Application
{
public:
    TerrainDemo(HINSTANCE hCurrentInst, const std::wstring &cmdLine);
    virtual ~TerrainDemo();

    // Platform::Application overrides
    virtual bool run();

    // Platform::EventHanler overrides
    virtual void TerrainDemo::moveEvent(Platform::Window* window, const std::string& event, int absX, int absY, int dx, int dy);
    virtual void buttonEvent(Platform::Window* window, __int64 virtualKeyCode, bool pressed);
    virtual void buttonEvent(Platform::Window* window, const std::string& event, bool pressed);
    virtual void resize(int width, int height);

    virtual bool screenSaver() {
        return false;
    }

private:
    void initOpenGL();
    void drawNextFrame();

    HINSTANCE _hInstance;

    Viewer* _viewer;
    Terrain* _terrain;
    SpriteRenderer* _spriteRenderer;
    TextRenderer* _textRenderer;

    float _sightx, _sighty;
    int _mousex, _mousey;
    int _windowsizex;
    int _windowsizey;
};

#endif
