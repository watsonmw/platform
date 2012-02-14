#include "stdafx.h"

#include "platform/openglwindow.h"

#include <string>
#include <map>

#include <gl/gl.h>

#include "platform/log.h"
#include "platform/eventhandler.h"

using namespace std;

static const int defaultWindowDepth = 32;

Platform::OpenGLWindow::OpenGLWindow
	(const wstring& name, Platform::EventHandler &eventHandler, int colorBits)
  : Platform::Window(name, eventHandler),
    _bNewRenderingContext(false),
    _colorBits(colorBits),
    _bUseAccelerationIfAvailable(true),
    _bChangedScreenRes(false),
    _hDC(NULL)
{
    if (_colorBits != 16 ||
        _colorBits != 24 ||
        _colorBits != 32) {
        _colorBits = DEFAULT_DISPLAY_DEPTH;
	}
}

Platform::OpenGLWindow::~OpenGLWindow()
{
}

bool Platform::OpenGLWindow::isNewRenderingContext()
{
    if (_bNewRenderingContext) {
        _bNewRenderingContext = false;
        return true;
    }
    return false;
}

void Platform::OpenGLWindow::close()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(_hRC);
    ReleaseDC(_hWnd, _hDC);

    Window::close();

    if (_bChangedScreenRes) {
        ChangeDisplaySettings(NULL, 0);
        _bChangedScreenRes = false;
    }
}

bool Platform::OpenGLWindow::openFullScreen(int width, int height)
{
    if (!changeDisplayMode(width, height, _colorBits)) {
        return false;
	}

    return Window::openFullScreen();
}

bool Platform::OpenGLWindow::createWindow
	(HWND parent, int xPos, int yPos, int width, int height, WindowType windowType)
{
    if (!Window::createWindow(parent, xPos, yPos, width, height, windowType)) {
        return false;
	}

    if (!setupPixelFormalAndRenderContext()) {
        return false;
	}

    return true;
}

bool Platform::OpenGLWindow::setupPixelFormalAndRenderContext()
{
    _hDC = GetDC(_hWnd);
    if (_hDC == NULL) {
        Log::print(L"Error: Unable to get Window Device Context!");
        return false;
    }

    int depth = _colorBits;
    if (depth == -1) {
        DEVMODE devmode;
        BOOL modeswitch =
            EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

        if (!modeswitch) {
            Log::print(L"Error: Unable to get current display resolution!");
            return false;
        }
        depth = devmode.dmBitsPerPel;
    }

    int pixelFormatIndex = choosePixelFormat
                           (_hDC, _bUseAccelerationIfAvailable, depth);

    if (pixelFormatIndex == -1) {
        Log::print(L"Error: Unable to find a suitable pixel format!");
        return false;
    }

    if (SetPixelFormat(_hDC, pixelFormatIndex, &_pixelFormat) == FALSE) {
        Log::print(L"Error: Unable to set PixelFormat!");
        return false;
    }

    clear();

    Log::print(L"Creating GL Context...");
    _hRC = wglCreateContext(_hDC);

    if (_hRC == NULL) {
        Log::print(L"Error: Failed to get rendering context!");
        return false;
    }

    Log::print(L"Making Context Current...");
    wglMakeCurrent(_hDC, _hRC);

    _bNewRenderingContext = true;

    const GLubyte *vendor   = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version  = glGetString(GL_VERSION);

    Log::printf(L"Using OpenGL %s %s %s", version, vendor, renderer);

    Log::print(L"OpenGL Rendering Window Created.");
    return true;
}

int Platform::OpenGLWindow::choosePixelFormat(HDC hdc, bool bUseAccelerationIfAvailable, int colorBits)
{
    //
    // Stereo formats can cause problems on some cards
    //
    const bool no_stereo = true;
    const int essentialFlags = PFD_DRAW_TO_WINDOW |
                               PFD_SUPPORT_OPENGL |
                               PFD_DOUBLEBUFFER;

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;

    int numberOfPixelFormats = DescribePixelFormat(hdc, 1, sizeof(pfd), &pfd);

    int bestFitSoFar = -1;
    int depth = 0;
    int color = 0;
    bool icdAcceleration = false;
    bool mcdAcceleration = false;

    for (int i = 1; i <= numberOfPixelFormats; i++) {
        ZeroMemory(&pfd, sizeof(pfd));
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;

        DescribePixelFormat(hdc, i, sizeof(pfd), &pfd);

        bool soft = ((pfd.dwFlags & PFD_GENERIC_FORMAT) &&
                     !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));

        bool mcd  = ((pfd.dwFlags & PFD_GENERIC_FORMAT) &&
                     (pfd.dwFlags & PFD_GENERIC_ACCELERATED));

        bool icd  = (!(pfd.dwFlags & PFD_GENERIC_FORMAT) &&
                     !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));

        if ((pfd.dwFlags & essentialFlags)
                != essentialFlags) {
            continue;
		}

        if (pfd.iPixelType != PFD_TYPE_RGBA) {
            continue;
		}

        if (pfd.iLayerType != PFD_MAIN_PLANE) {
            continue;
		}

        if (pfd.cColorBits < color) {
            continue;
		}

        if (color > colorBits) {
            continue;
		}

        if (!bUseAccelerationIfAvailable) {
            if (mcd || icd)
                continue;
        }

        if (bestFitSoFar == -1) {
            depth = pfd.cDepthBits;
            icdAcceleration = icd;
            mcdAcceleration = mcd;
        }

        if (icdAcceleration && soft) {
            continue;
		}

        if (mcdAcceleration && icd) {
            continue;
		}

        if (depth > pfd.cDepthBits) {
            continue;
		}

        if (no_stereo && pfd.dwFlags & PFD_STEREO) {
            continue;
		}

        bestFitSoFar = i;
        color = pfd.cColorBits;
        depth = pfd.cDepthBits;
        icdAcceleration = icd;
        mcdAcceleration = mcd;

        _pixelFormat = pfd;
    }

    return bestFitSoFar;
}

bool Platform::OpenGLWindow::changeDisplayMode(int width, int height, int colorBits)
{
    Log::printf(L"Changing Display Mode: %d*%d*%d@naHz", width, height, colorBits);

    DEVMODE devmode;
    devmode.dmBitsPerPel = 0;
    int modeswitch;
    int i = 0;

    do {
        modeswitch = EnumDisplaySettings(NULL, i, &devmode);
        if (!modeswitch) {
            Log::print(L"Warning: Unable to find requested screen mode.");
            break;
        }
        i++;
    }
    while ((devmode.dmBitsPerPel != colorBits) ||
            (devmode.dmPelsWidth  != width) ||
            (devmode.dmPelsHeight != height) ||
            (ChangeDisplaySettings(&devmode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL));

    if (modeswitch) {
        Log::print(L"Using Requested Display Mode.");
        ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
        _bChangedScreenRes = true;
        return true;
    }

    Log::print(L"Using Display Mode Hack.");
    devmode.dmBitsPerPel = colorBits;
    devmode.dmPelsWidth  = width;
    devmode.dmPelsHeight = height;
    devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    modeswitch = ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);

    if (modeswitch != DISP_CHANGE_SUCCESSFUL) {
        Log::print(L"Error: Failed to Change Display Mode!");
        return false;
    }

    _bChangedScreenRes = true;
    return true;
}


void Platform::OpenGLWindow::swapBuffers()
{
    SwapBuffers(_hDC);
}


void Platform::OpenGLWindow::useAccelerationIfAvailable(bool accel)
{
    _bUseAccelerationIfAvailable = accel;
}
