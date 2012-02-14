#include "stdafx.h"

#include "platform/window.h"

#include "assert.h"
#include <string>
#include <map>

#include "platform/log.h"
#include "platform/eventhandler.h"

using namespace std;

static const int sMaxWindowWidth = 10000;
static const int sDefaultWindowWidth = 800;
static const int sMinWindowWidth = 60;

static const int sMaxWindowHeight = 10000;
static const int sDefaultWindowHeight = 600;
static const int sMinWindowHeight = 40;

static map<HWND, Platform::Window *> sThisPointers;
static bool sbWindowClassRegistered = false;

Platform::Window::Window
(const wstring &name, Platform::EventHandler &eventHandler, bool bDoubleBuffered)
  : _eventHandler(eventHandler),
    _bDoubleBuffered(bDoubleBuffered),
    _name(name),
    _hInstance(0),
    _bWindowOpen(false),
    _height(0),
    _width(0),
    _mousex(0),
    _mousey(0),
    _uniqueWindowTimerId(10000), // our USER timer ids start at 10000
    _doubleBufferBackDc(NULL),
    _doubleBufferBackBmp(NULL),
    _doubleBufferOldBmp(NULL)
{
}

Platform::Window::~Window()
{
    SetCursor(_hcursor);
    removeDoubleBuffer();
}

bool Platform::Window::open(HWND parent)
{
    RECT rect;
    GetWindowRect(parent, &rect);

    if (!createWindow(parent, 0, 0,
                      rect.right  - rect.left,
                      rect.bottom - rect.top,
                      CHILD_WindowType)) {
        return false;
    }

    return true;
}


bool Platform::Window::open(int width, int height)
{
    return createWindow(NULL, DefaultX, DefaultY,
                        width, height, TOPLEVEL_WindowType);
}


bool Platform::Window::openFullScreen()
{
    DEVMODE devmode;
    BOOL gotScreenMode =
        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

    if (!gotScreenMode) {
        Log::print(L"Warning: Unable to find the current screen size.");
        return false;
    }

    if (!createWindow(NULL,
                      0,
                      0,
                      devmode.dmPelsWidth,
                      devmode.dmPelsHeight,
                      FULLSCREEN_WindowType))
        return false;

    ShowCursor(FALSE);

    return true;
}


bool Platform::Window::openFullScreen(HMONITOR monitor)
{
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    BOOL gotMonitorInfo = GetMonitorInfo(monitor, &monitorInfo);
    if (!gotMonitorInfo) {
        Log::print(L"Warning: Unable to find desktop position of monitor.");
        return false;
    }

    if (!createWindow(NULL,
                      monitorInfo.rcMonitor.left,
                      monitorInfo.rcMonitor.top,
                      monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                      monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                      FULLSCREEN_WindowType)) {
        return false;
    }

    ShowCursor(FALSE);

    return true;
}

void Platform::Window::clear(HDC hDC)
{
    bool bReleaseDC = false;
    if (hDC == NULL) {
        hDC = GetDC(_hWnd);
        bReleaseDC = true;
    }
    if (hDC) {
        PatBlt(hDC, 0, 0, _width, _height, BLACKNESS);
        if (bReleaseDC) {
            ReleaseDC(_hWnd, hDC);
        }
    }
}

bool Platform::Window::createWindow(HWND parent,
                                    int xPos, int yPos,
                                    int width, int height,
                                    WindowType windowType)
{
    if (width < sMinWindowWidth ||
            width > sMaxWindowWidth) {
        width = sDefaultWindowWidth;
    }

    if (height < sMinWindowHeight ||
            height > sMaxWindowHeight) {
        height = sDefaultWindowHeight;
    }

    _parent = parent;
    _width  = width;
    _height = height;

    // Reopen
    if (_bWindowOpen) {
        close();
    }

    Log::print(L"Creating Window...");

    _hInstance = GetModuleHandle(NULL);
    if (!registerWindow(L"PlatformBasicWindow")) {
        return false;
    }

    _bFirstMouseEvent = true;

    RECT windowRect = {0, 0, width, height};
    DWORD windowStyle = 0;
    DWORD windowExtendedStyle = 0;

    switch (windowType) {
    case TOPLEVEL_WindowType:
        //
        // Make top level window
        //
        windowStyle = WS_OVERLAPPEDWINDOW;
        AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
        break;
    case CHILD_WindowType:
        //
        // Make window inside parent
        //
        windowStyle = WS_CHILD;
        break;
    case FULLSCREEN_WindowType:
        //
        // Make fullscreen window
        //
        windowStyle = WS_OVERLAPPEDWINDOW;
        windowExtendedStyle = WS_EX_APPWINDOW;
        windowStyle = WS_POPUP;
#ifndef _DEBUG // Leave out in debug mode as it can make debugger
        // breaks hard to see
        windowExtendedStyle |= WS_EX_TOPMOST;
#endif
        break;
    }

    _hWnd = CreateWindowEx(windowExtendedStyle,
                           L"PlatformBasicWindow",
                           _name.c_str(),
                           windowStyle,
                           (xPos == DefaultX) ? CW_USEDEFAULT : xPos,
                           (yPos == DefaultY) ? CW_USEDEFAULT : yPos,
                           windowRect.right - windowRect.left,
                           windowRect.bottom - windowRect.top,
                           parent,
                           0,
                           _hInstance,
                           NULL);

    if (_hWnd == NULL) {
        Log::print(L"Error: CreateWindow Failed!");
        return false;
    }

    sThisPointers[_hWnd] = this;
    _bWindowOpen = true;

    Log::print(L"Showing Window...");
    ShowWindow(_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(_hWnd);

    if (windowType == CHILD_WindowType) {
        SetFocus(_hWnd);

        //
        // Remember the old cursor and set the current cursor to nothing
        //
        HCURSOR old_cursor;
        if ((old_cursor = SetCursor(NULL)) != NULL) {
            _hcursor = old_cursor;
        }
    }

    return true;
}

void Platform::Window::close()
{
    Log::print(L"Destroying Window...");

    if (_hWnd != NULL) {
        sThisPointers.erase(_hWnd);
    }

    DestroyWindow(_hWnd);

    _bWindowOpen = false;
}

bool Platform::Window::registerWindow(const wstring &windowClassName)
{
    if (!sbWindowClassRegistered) {
        //
        // register window class
        //
        WNDCLASSEX wndClass;
        ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
        wndClass.cbSize        = sizeof(WNDCLASSEX);
        wndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wndClass.lpfnWndProc   = WndProc;
        wndClass.hInstance     = _hInstance;
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.hIcon         = LoadIcon(_hInstance, NULL);
        wndClass.hCursor       = LoadCursor(0, IDC_ARROW);
        wndClass.lpszClassName = windowClassName.c_str();

        if (!RegisterClassEx(&wndClass)) {
            Log::print(L"Error: RegisterClass Failed!");
            return false;
        }

        sbWindowClassRegistered = true;
    }

    return true;
}

int Platform::Window::getNewTimerId()
{
    _uniqueWindowTimerId++;
    return _uniqueWindowTimerId;
}

void Platform::Window::startTimer(int timerId, int milliseconds)
{
    SetTimer(_hWnd, timerId, milliseconds, (TIMERPROC)NULL);
}

void Platform::Window::stopTimer(int timerId)
{
    KillTimer(_hWnd, timerId);
}

void Platform::Window::sendPaintEvent()
{
    InvalidateRect(_hWnd, NULL, FALSE);
}

int windowsKeyCodeToAscii(WPARAM key)
{
    int vKey = LOBYTE(key);
    int nScan = HIBYTE(key);
    nScan <<= 16;

    BYTE kbuf[256];
    WORD ch;
    int chcount;

    GetKeyboardState(kbuf);

    chcount = ToAscii(vKey, nScan, kbuf, &ch, 0);

    return (int)ch;
}

bool Platform::Window::dispatchEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    // EventHandler _eventHandler;
    switch (message) {
    case WM_SYSCOMMAND:
        switch (wParam) {
        case SC_SCREENSAVE:
            return _eventHandler.screenSaver();
        case SC_MONITORPOWER:
            return _eventHandler.monitorPower();
        }
        break;
    case WM_QUIT:
        return true;
    case WM_CLOSE:
        PostQuitMessage(0);
        return true;
    case WM_CREATE:
        break;
    case WM_CHAR:
        /* handle keyboard input */
        switch (wParam) {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_KEYDOWN:
        _eventHandler.buttonEvent(this, wParam, true);
        break;
    case WM_KEYUP:
        _eventHandler.buttonEvent(this, wParam, false);
        break;
    case WM_LBUTTONDOWN:
        _eventHandler.buttonEvent(this, "mouse1", true);
        break;
    case WM_LBUTTONUP:
        _eventHandler.buttonEvent(this, "mouse1", false);
        break;
    case WM_RBUTTONDOWN:
        _eventHandler.buttonEvent(this, "mouse2", true);
        break;
    case WM_RBUTTONUP:
        _eventHandler.buttonEvent(this, "mouse2", false);
        break;
    case WM_MBUTTONDOWN:
        _eventHandler.buttonEvent(this, "mouse3", true);
        break;
    case WM_MBUTTONUP:
        _eventHandler.buttonEvent(this, "mouse3", false);
        break;
    case WM_MOUSEWHEEL:
        _eventHandler.buttonEvent(this, "mousewheel", true);
        break;
    case WM_MOUSEMOVE: {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        if (_bFirstMouseEvent) {
            _bFirstMouseEvent = false;
        }
        else {
            _eventHandler.moveEvent(this,
                                    "mousemove",
                                    _mousex,
                                    _mousey,
                                    xPos - _mousex,
                                    yPos - _mousey);
        }
        _mousex = xPos;
        _mousey = yPos;
    }
    break;
    case WM_TIMER:
        _eventHandler.timer(this, (int)wParam);
        break;
    case WM_PAINT:
        return _eventHandler.paint(this);
    case WM_SIZE:
        _eventHandler.resize(LOWORD(lParam), HIWORD(lParam));
        break;
    default:
        return false;
    }

    return false;
}

wstring Platform::Window::getKeyName(WPARAM virtualKeyCode)
{
    TCHAR buffer[1024];
    GetKeyNameText((LONG)virtualKeyCode, buffer, 1024);
    return buffer;
}

LRESULT APIENTRY Platform::Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    map<HWND, Window *>::iterator i = sThisPointers.find(hWnd);

    if (i == sThisPointers.end()) {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    Window *window = (*i).second;
    if (window->dispatchEvent(message, wParam, lParam)) {
        return 0L;
    }
    else {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

bool Platform::Window::setupDoubleBuffer(HDC hDc)
{
    GetClientRect(_hWnd, &_doubleBufferWindowRect); // Get the client RECT

    _doubleBufferBackDc = CreateCompatibleDC(hDc);

    if (_doubleBufferBackDc == NULL) {
        return false;
    }

    _doubleBufferBackBmp = CreateCompatibleBitmap(hDc, _doubleBufferWindowRect.right, _doubleBufferWindowRect.bottom);

    if (_doubleBufferBackBmp == NULL) {
        return false;
    }

    // Save the old bitmap as we'll want to set this back when finished
    // so that its cleaned up in the proper place.
    _doubleBufferOldBmp  = (HBITMAP)SelectObject(_doubleBufferBackDc, _doubleBufferBackBmp);
    return true;
}

void Platform::Window::removeDoubleBuffer()
{
    if (_doubleBufferOldBmp) { // If we have a double buffer
        // Select back the original "bitmap"
        SelectObject(_doubleBufferBackDc, _doubleBufferOldBmp);

        // Free up memory
        DeleteObject(_doubleBufferBackBmp);
        DeleteDC(_doubleBufferBackDc);
    }
}

HDC Platform::Window::beginPaint(PAINTSTRUCT *paintInfo)
{
    HDC hdc = BeginPaint(_hWnd, paintInfo);
    if (_bDoubleBuffered) {
        if (_doubleBufferBackDc) {
            return _doubleBufferBackDc;
        }
        else if (setupDoubleBuffer(hdc)) {
            return _doubleBufferBackDc;
        }
    }

    return hdc;
}

void Platform::Window::endPaint(PAINTSTRUCT *paintInfo)
{
    if (_bDoubleBuffered) {
        HDC hdc = GetDC(_hWnd);
        BitBlt(hdc, 0, 0,
               _doubleBufferWindowRect.right,
               _doubleBufferWindowRect.bottom,
               _doubleBufferBackDc,
               0, 0, SRCCOPY);
        ReleaseDC(_hWnd, hdc);
    }
    EndPaint(_hWnd, paintInfo);
}
