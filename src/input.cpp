#include "stdafx.h"

#include "platform/input.h"
#include "platform/openglwindow.h"
#include "platform/log.h"

#include <string>
using namespace std;

#include <d3dx8.h>
#include <dinput.h>

class InputPimpl
{
public:
    //DI Object
    LPDIRECTINPUT8 directInput;
    LPDIRECTINPUTDEVICE8 keyboardDevice;
};

Input::Input(OpenGLWindow *)
{
    LPDIRECTINPUT8 directInput;
    LPDIRECTINPUTDEVICE8 keyboardDevice;

    // Create a DInput object
    //  if( FAILED(DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,
    //                                 IID_IDirectInput8, (void **)&directInput, NULL ) ) )
    //    throw;

    // Obtain an interface to the system keyboard device.
    //  if( FAILED(directInput->CreateDevice( GUID_SysKeyboard, &keyboardDevice, NULL ) ) )
    //    throw;

    pimpl = new InputPimpl();
    pimpl->directInput = directInput;
    pimpl->keyboardDevice = keyboardDevice;
}

Input::~Input()
{
    delete pimpl;
}

void Input::updateState()
{
}

Button *Input::getButton(const std::wstring &name)
{
    return new Button(0);
}

bool Input::isButtonUp(const Button *button)
{
    return false;
}

bool Input::isButtonReleased(const Button *button)
{
    return false;
}

bool Input::isButtonDown(const Button *button)
{
    return false;
}
