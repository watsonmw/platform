#ifndef _INPUT_DOT_H_INCLUDED_
#define _INPUT_DOT_H_INCLUDED_

#include <string>

class OpenGLWindow;

class Button
{
public:
    Button(int _id) : id(_id) {}
    int getId() {
        return id;
    }

private:
    int id;
};

class Input
{
public:
    Input(OpenGLWindow *);
    ~Input();

    void updateState();

    Button *getButton(const std::wstring &name);

    bool isButtonUp(const Button *button);
    bool isButtonReleased(const Button *button);
    bool isButtonDown(const Button *button);

private:
    class InputPimpl *pimpl;
};

#endif