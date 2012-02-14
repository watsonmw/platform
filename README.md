"platform" is a set of lightweight framework for writing graphical demos and screensavers on Windows.

The code abstracts away most Win32 internals, the idea being to keep most Win32-isms out of your projects, which is useful for porting or just improving the clarity of the code.

It contains utility classes for the following:

 - Easy way to write screen savers (A device context or an OpenGL context is provided to render into).
 - Window creation and event interception
 - OpenGL render context creation, both fullscreen and windowed
 - Mouse and keyboard input
 - Storing and reading settings from a file or the registry
 - Threading
 - Parsing command line switches
 - Load image files
 - Debug logging
 - Directory listings/file info

Example programs are included under the "examples" directory.

   PhotoViewer - A GDI screen saver that displays images from the local drive.

   Matrix - A OpenGL screen saver that displays matrix style falling codes.

   TerrainDemo - An OpenGL application with a startup dialog, that demonstrates both fullscreen and windowed OpenGL.  The keyboard and mouse are used to control the camera.
