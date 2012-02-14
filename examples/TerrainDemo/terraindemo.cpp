#include "stdafx.h"
#include "terraindemo.h"
#include "draw2d.h"
#include "resource.h"
#include "options.h"
#include "viewer.h"
#include "terrain.h"

#include "platform/openglwindow.h"
#include "platform/log.h"

using namespace Platform;

static int sScreenres = 0;
static bool sbFullscreen = false;
static bool sbSixteenbit = false;

static unsigned int sRes[4][2] = {
    {800, 600},
    {640, 480},
    {512, 384},
    {320, 240}
};

BOOL CALLBACK SetupDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static int ires;

    switch (Message) {
    case WM_INITDIALOG:
        ires  = IDC_RES800;

        CheckRadioButton(hWnd, IDC_RES800, IDC_RES320, ires);
        SetFocus(GetDlgItem(hWnd, ires));

        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            sScreenres = ires - 1000;
            EndDialog(hWnd, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
        case IDC_ABOUT:
            MessageBox(hWnd, L"Screen Setup (c) 2000 Mark Watson.", L"About", MB_OK);
            break;
        case IDC_FULLSCREEN:
            sbFullscreen = !sbFullscreen;
            return TRUE;
        case IDC_16BIT:
            sbSixteenbit = !sbSixteenbit;
            return TRUE;
        case IDC_RES800:
        case IDC_RES640:
        case IDC_RES512:
        case IDC_RES320:
            ires = LOWORD(wParam);
            CheckRadioButton(hWnd, IDC_RES800, IDC_RES320, ires);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

TerrainDemo::TerrainDemo(HINSTANCE hInstance, const std::wstring &cmdLine)
  : Application(L"TerrainDemo", cmdLine),
    _hInstance(hInstance),
    _viewer(NULL),
    _terrain(NULL),
    _spriteRenderer(NULL),
    _textRenderer(NULL),
    _sightx(0.0),
    _sighty(0.0),
    _mousex(0),
    _mousey(0),
    _windowsizex(0),
    _windowsizey(0)
{
}

TerrainDemo::~TerrainDemo()
{
    delete _viewer; _viewer = 0;
    delete _terrain; _terrain = 0;
    delete _spriteRenderer; _spriteRenderer = 0;
    delete _textRenderer; _textRenderer = 0;
}

void TerrainDemo::initOpenGL()
{
    _viewer = new Viewer();
    _terrain = new Terrain();
    _terrain->load(L"data/terrain.tga");

    _spriteRenderer = new SpriteRenderer();
    _spriteRenderer->load();

    _textRenderer = new TextRenderer();
    _textRenderer->load(L"data/chars.pcx");

    GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };
    GLuint fogfilter = 0;
    GLfloat fogColor[4] = {0.4f, 0.0f, 0.0f, 0.0f};

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glFogi(GL_FOG_MODE, fogMode[fogfilter]);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.1f);
    glHint(GL_FOG_HINT,  GL_DONT_CARE );
    glFogf(GL_FOG_START, 20.0f);
    glFogf(GL_FOG_END, 400.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glShadeModel(GL_SMOOTH);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glAlphaFunc(GL_GREATER, 0.5);
}

bool TerrainDemo::run() {
    int frames = 0;
    float fps;
    DWORD time;

    DWORD ret = DialogBox(_hInstance, L"SETUPOPTIONS", NULL, (DLGPROC) SetupDlgProc);
    if (ret == IDCANCEL) {
        Log::print("Setup canceled.");
        return false;
    }
        
    int screendepth = sbSixteenbit ? 16 : 32;
    int screenwidth = sRes[sScreenres][0];
    int screenheight = sRes[sScreenres][1];

    OpenGLWindow openglwindow(L"TerrainDemo", *this, screendepth);
    if (sbFullscreen) {
        if (!openglwindow.openFullScreen(screenwidth, screenheight)) {
            Log::print("Failed to create suitable fullscreen rendering context.");
            return false;
        }
    }
    else {
        if (!openglwindow.open(screenwidth, screenheight)) {
            Log::print("Failed to create suitable rendering context.");
            return false;
        }
    }

    Log::printf("Using %s's \"%s %s\" OpenGL Library",
             glGetString(GL_VENDOR),
             glGetString(GL_RENDERER),
             glGetString(GL_VERSION));

    Log::printf("Extensions: %s", glGetString(GL_EXTENSIONS));

    initOpenGL();
    resize(openglwindow.getWidth(), openglwindow.getHeight());

    time = timeGetTime();

    while (processPendingEvents()) {
        drawNextFrame();
        openglwindow.swapBuffers();
        frames++;
    }

    time = timeGetTime() - time;
    fps = (float)(frames) / ((float)(time) * 0.001f);
    Log::printf("Frames: %d Time %d Fps: %f.", frames, time, fps);
    openglwindow.close();
    return true;
}

void TerrainDemo::resize(int x, int y)
{
    _windowsizex = x;
    _windowsizey = y;

    if (!_viewer) {
        return;
    }

    // ReInitialize Projection and
    // ModelView Matrices to adapt to
    // new window size.

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //initialize the PROJECTION matrix

    if (y != 0) {
        _viewer->ar = (GLfloat)x / (GLfloat)y;
        gluPerspective(30.0,        // View Angle
                       _viewer->ar, // aspect ratio
                       1,           // distance to front clipping plane
                       2048);       // distance to back clipping plane

        glViewport(0, 0, x, y);
        //Set the view port to be entire screen
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void TerrainDemo::drawNextFrame()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0,        //View Angle
                   _viewer->ar,  //aspect ratio
                   1,           //distance to front clipping plane
                   2048);       //distance to back clipping plane

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _viewer->draw(_terrain);

    glColor3f(1.0f, 1.0f, 1.0f);

    _terrain->Draw(_viewer);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, _windowsizex, _windowsizey, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glTranslatef(0.375, 0.375, 0.0);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_CULL_FACE);

    _textRenderer->drawString(_terrain->placename, 20, 20);

    if (opt_opengl_pointer) {
        _spriteRenderer->drawSprite("cursor", _mousex - 8, _mousey - 8, _mousex + 8, _mousey + 8);
    }

    glDisable(GL_ALPHA_TEST);
    glEnable (GL_CULL_FACE);

    glFlush();
}

void TerrainDemo::moveEvent(Platform::Window* window, const std::string& event, int absX, int absY, int dx, int dy)
{
    float rx = (2.0f * absX / _windowsizex) - 1;
    float ry = (2.0f * absY / _windowsizey) - 1;

    if (_viewer) {
        _viewer->onMouseMove(absX, absY, dx, dy, rx, ry);
    }

    _mousex = absX;
    _mousey = absY;
}

void TerrainDemo::buttonEvent(Platform::Window* window, __int64 virtualKeyCode, bool pressed)
{
    if (pressed) {
        switch (virtualKeyCode) {
        case 'W':
            _viewer->moveForward = true;
            break;
        case 'S':
            _viewer->moveBack = true;
            break;
        case 'A':
            _viewer->moveLeft = true;
            break;
        case 'D':
            _viewer->moveRight = true;
            break;
        case '1':
            _viewer->setMode(Viewer::MoveMode(((int)_viewer->mode + 1) % 3));
            break;
        case '2':
            opt_terrain_wireframe = !opt_terrain_wireframe;
            break;
        case '3':
            opt_sky_draw = !opt_sky_draw;
            break;
        case '4':
            opt_global_debug = !opt_global_debug;
            break;
        case '5':
            opt_global_night = !opt_global_night;
            if (opt_global_night) {
                glEnable(GL_FOG);
            }
            else {
                glDisable(GL_FOG);
            }
            break;
        case '6':
            opt_opengl_pointer = !opt_opengl_pointer;
            break;
        }
    }
    else {
        switch (virtualKeyCode) {
        case 'W':
            _viewer->moveForward = false;
            break;
        case 'S':
            _viewer->moveBack = false;
            break;
        case 'A':
            _viewer->moveLeft = false;
            break;
        case 'D':
            _viewer->moveRight = false;
            break;
        }
    }
}

void TerrainDemo::buttonEvent(Platform::Window* window, const std::string& event, bool pressed)
{
    if (event == "mouse1") {
        if (pressed) {
            _viewer->setMode(Viewer::MOVETOAREAOFINTEREST_MoveMode);
        } else {
            _viewer->setMode(Viewer::AREAOFINTEREST_MoveMode);
        }
    } else if (event == "mouse2") {
        if (pressed) {
            _viewer->setMode(Viewer::MOUSELOOK_MoveMode);
        } else {
            _viewer->setMode(Viewer::AREAOFINTEREST_MoveMode);
        }
    }
}
