Controls:

Move focus area around using the mouse.
Left mouse button move the camera to focus.  Click on the terrain to rotate the camera to look directly at that point.  Press and hold to smoothly move the camera around.
Right mouse button to look around (freelook/mouselook) 

Keys:
Esc - Quits.
W   - Move forward
S   - Move back
A   - Move left
D   - Move right
1   - Toggle mouse mode
2   - Switches wireframe on/off
3   - Show sky box

For screenshots use a full implementation of OpenGL, hardware accelerated implementations may not offer the features needed for a nice wire frame overlay.

Go to options.h and undefine NICE_WIREFRAME if necessary.

If your card does support OpenGL, or the implementation doesn't support the features needed try www.altsoftware.com for a OpenGL DirectX wrapper.
