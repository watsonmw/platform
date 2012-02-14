#ifndef _PLATFORM_OPENGLWINDOW_INCLUDED_H_
#define _PLATFORM_OPENGLWINDOW_INCLUDED_H_

#include "platform/window.h"

namespace Platform
{
    class OpenGLWindow : public Window
    {
    public:
        enum { DEFAULT_DISPLAY_DEPTH = 24 };

        OpenGLWindow(const std::wstring &name,
                     EventHandler &eventHandler,
                     int colorBits = DEFAULT_DISPLAY_DEPTH);
        virtual ~OpenGLWindow();

        /**
         * Open a Window at fullscreen size on the current display
         */
        bool openFullScreen() {
            return Window::openFullScreen();    // Only need this here because of
                                                // stupid compiler not seeing
                                                // Window::openFullScreen()
        }

        /**
         * Changes the display resolution and opens a fullscreen window
         * on the current display
         */
        bool openFullScreen(int width, int height);

        /**
         * Close the window and any resources associated with it.
         */
        void close();

        /**
         * Double buffer swap
         */
        void swapBuffers();

        /**
         * Checks if a new rendering context is available, if it is
         * you need to load all the opengl resources again.
         * The act of checking resets the internal flag, so only call
         * this from one place.
         */
        bool isNewRenderingContext();

        /**
         * Use graphics acceleration next time a graphics context is created.
         */
        void useAccelerationIfAvailable(bool accel);

    protected:
        bool createWindow(HWND parent, int xPos, int yPos, int width, int height, WindowType windowType);

        bool setupPixelFormalAndRenderContext();

        bool changeDisplayMode(int width, int height, int colorBits);

        int choosePixelFormat(HDC hdc, bool useAccelerationIfAvailable, int colorBits);

        bool dispatchEvent(UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT APIENTRY WndProc(HWND hWnd,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam);

        bool _bNewRenderingContext;
        bool _bChangedScreenRes;
        bool _bUseAccelerationIfAvailable;

        int _colorBits;
        HGLRC _hRC;
        HDC _hDC;
        PIXELFORMATDESCRIPTOR _pixelFormat;
    };
};

#endif