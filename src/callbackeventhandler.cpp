#include "stdafx.h"
#include "platform/callbackeventhandler.h"

using namespace std;

namespace Platform
{
    class Window;
    class Callback
    {
    public:
        Callback(void *_instance_ptr, void (*_function)(void *))
            : function(_function), instance_ptr(_instance_ptr) {};

        void call() {
            function(instance_ptr);
        }

    private:
        void (*function)(void *);
        void *instance_ptr;
    };

    class BoolCallback
    {
    public:
        BoolCallback(void *_instance_ptr, void (*_function)(void *, bool))
            : function(_function), instance_ptr(_instance_ptr) {};

        void call(bool _bool) {
            function(instance_ptr, _bool);
        }

    private:
        void (*function)(void *, bool);
        void *instance_ptr;
    };

    CallbackEventHandler::CallbackEventHandler()
    {
    }

    CallbackEventHandler::~CallbackEventHandler()
    {
    }

    void CallbackEventHandler::moveEvent(Window *window, const std::string &event, int x, int y)
    {
    }

    void CallbackEventHandler::buttonEvent(Window *window, const std::string &event, bool pressed)
    {
        if (button_actions.count(event) == 1) {
            button_actions[event]->call(pressed);
        }
    }

    void CallbackEventHandler::buttonEvent(Window *window, char event, bool pressed)
    {
        string dispatch_event;
        dispatch_event += event;
        buttonEvent(window, dispatch_event, pressed);
    }

    void CallbackEventHandler::registerAction(const std::string &action,
            void *instance_ptr,
            void (*function)(void *))
    {
        actions[action] = new Callback(instance_ptr, function);
    }

    void CallbackEventHandler::registerButtonAction(const std::string &action,
            void *instance_ptr,
            void (*function)(void *, bool))
    {
        button_actions[action] = new BoolCallback(instance_ptr, function);
    }
};