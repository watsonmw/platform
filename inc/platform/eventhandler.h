#ifndef _PLATFORM_EVENTHANDLER_H_INCLUDED_
#define _PLATFORM_EVENTHANDLER_H_INCLUDED_

#include <string>

namespace Platform
{
    class Window;

    class EventHandler
    {
    public:
        virtual void moveEvent(Window *window, const std::string &event, int absX, int absY, int dx, int dy) {};
        virtual void buttonEvent(Window *window, __int64 virtualKeyCode, bool pressed) {};
        virtual void buttonEvent(Window *window, const std::string &event, bool pressed) {};

        /**
         * Called when the current screensaver is about to be run
         * Return false if you dont want it to run.
         */
        virtual bool screenSaver() {
            return true;
        }

        /**
         * Called when the monitor is about to go into power save
         * Return false if you dont want the monitor to switch off
         */
        virtual bool monitorPower() {
            return true;
        }

        /**
         * Called when we get a paint event
         */
        virtual bool paint(Window *window) {
            return false;
        }
            
        /**
         * Called when we get a resize event
         */
        virtual void resize(int width, int height) {}

        /**
         * Called when a timer is activated
         */
        virtual void timer(Window *window, int timerId) {}
    };
};

#endif