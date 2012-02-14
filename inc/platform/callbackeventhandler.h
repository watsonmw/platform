#ifndef _PLATFORM_CALLBACKEVENTHANDLER_H_INCLUDED_
#define _PLATFORM_CALLBACKEVENTHANDLER_H_INCLUDED_

#include "platform/eventhandler.h"

#include <map>

namespace Platform
{
    class Callback;
    class BoolCallback;

    class CallbackEventHandler : public EventHandler
    {
    public:
        CallbackEventHandler();
        virtual ~CallbackEventHandler();

        virtual void moveEvent(Window *window, const std::string &event, int x, int y);
        virtual void buttonEvent(Window *window, char event, bool pressed);
        virtual void buttonEvent(Window *window, const std::string &event, bool pressed);

        void registerAction(const std::string &,
                            void *instance_ptr,
                            void (*function)(void *));

        void registerButtonAction(const std::string &,
                                  void *instance_ptr,
                                  void (*function)(void *, bool));

    private:
        std::map<std::string, Callback *> actions;
        std::map<std::string, BoolCallback *> button_actions;
    };
};

#endif