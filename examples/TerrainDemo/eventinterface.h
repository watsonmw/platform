#ifndef EVENTINTERFACE_DOT_H
#define EVENTINTERFACE_DOT_H

class EventInterface
{
public:
    virtual void Resize(int x, int y) = 0;
    virtual void OnRightMouseDown() = 0;
    virtual void OnRightMouseUp() = 0;
    virtual void OnLeftMouseDown() = 0;
    virtual void OnLeftMouseUp() = 0;
    virtual void OnMouseMove(int x, int y) = 0;

    virtual void OnKeyUp(int nChar) = 0;
    virtual void OnKeyDown(int nChar) = 0;
};

#endif