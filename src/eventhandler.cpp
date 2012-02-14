#include "stdafx.h"
#include "platform/eventhandler.h"

using namespace std;

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

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

void EventHandler::dispatchMoveEvent(const std::string &event, int x, int y)
{
}

void EventHandler::dispatchButtonEvent(const std::string &event, bool pressed)
{
    if (button_actions.count(event) == 1) {
        button_actions[event]->call(pressed);
    }
}

void EventHandler::dispatchButtonEvent(char event, bool pressed)
{
    string dispatch_event;
    dispatch_event += event;
    dispatchButtonEvent(dispatch_event, pressed);
}

void EventHandler::registerAction(const std::string &action,
                                  void *instance_ptr,
                                  void (*function)(void *))
{
    actions[action] = new Callback(instance_ptr, function);
}

void EventHandler::registerButtonAction(const std::string &action,
                                        void *instance_ptr,
                                        void (*function)(void *, bool))
{
    button_actions[action] = new BoolCallback(instance_ptr, function);
}