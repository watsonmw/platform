#include "stdafx.h"

#include "platform/directory.h"
#include "platform/filelist.h"
#include "platform/log.h"
#include "platform/window.h"
#include "platform/eventhandler.h"
#include "platform/settings.h"
#include "platform/parseargs.h"
#include "platform/screensaverapplication.h"
#include "platform/registrysettings.h"
#include "platform/thread.h"

#include "configdialog.h"
#include "textrenderer.h"
#include "photoviewerconfig.h"

#include <string>
#include <sstream>
#include <cmath>

#include <FreeImage.h>

using namespace std;
using namespace Platform;


/**
 * Thread who's purpose is to create a new image
 * scaled to fit into the specified size.
 */
class ResizeImageThread : public Platform::Thread
{
public:
    ResizeImageThread(FIBITMAP *image, int new_width, int new_height, FREE_IMAGE_FILTER rescale_filter)
      : orig_image(image),
        width(new_width),
        height(new_height),
        rescale_filter(rescale_filter),
        resized_image(NULL),
        duration(0)
    {
    }

    void run()
    {
        DWORD startTime = timeGetTime();

        // Risize the image to fit into the given size keeping the
        // aspect ratio the same.
        double width_ratio =  (double)FreeImage_GetWidth(orig_image) /
                              (double)width;

        double height_ratio = (double)FreeImage_GetHeight(orig_image) /
                              (double)height;

        int image_height = height;
        int image_width = width;

        if (height_ratio > width_ratio) {
            image_width  = (int)(FreeImage_GetWidth(orig_image) / height_ratio);
        }
        else {
            image_height = (int)(FreeImage_GetHeight(orig_image) / width_ratio);
        }

        // Free only supports rescaling of images in 8, 24 or 32 bits per pixel
        int bitsPerPixel = FreeImage_GetBPP(orig_image);
        if (bitsPerPixel == 8 ||
                bitsPerPixel == 24 ||
                bitsPerPixel == 32) {
            resized_image = FreeImage_Rescale(orig_image,
                                              image_width,
                                              image_height,
                                              rescale_filter);
        }
        else if (bitsPerPixel < 8) {
            FIBITMAP *image_8 = FreeImage_ConvertTo8Bits(orig_image);
            resized_image = FreeImage_Rescale(image_8,
                                              image_width,
                                              image_height,
                                              rescale_filter);
            FreeImage_Unload(image_8);
        }
        else if (bitsPerPixel == 16) {
            FIBITMAP *image_24 = FreeImage_ConvertTo24Bits(orig_image);
            resized_image = FreeImage_Rescale(image_24,
                                              image_width,
                                              image_height,
                                              rescale_filter);
            FreeImage_Unload(image_24);
        }
        else {
            FIBITMAP *image_32 = FreeImage_ConvertTo32Bits(orig_image);
            resized_image = FreeImage_Rescale(image_32,
                                              image_width,
                                              image_height,
                                              rescale_filter);
            FreeImage_Unload(image_32);
        }

        duration = timeGetTime() - startTime;
        Log::printf(L"Rescaling with filter %d took %d ms.", rescale_filter, duration);
    }

    /**
     * Retrieve the resized image after this thread has stopped.
     */
    FIBITMAP *getRisizedImage() { return resized_image; }

    /**
     * Returns the lenght of time the thread ran for.
     */
    int getDuration() { return duration; }

private:
    FIBITMAP *orig_image;
    FIBITMAP *resized_image;
    int width;
    int height;
    FREE_IMAGE_FILTER rescale_filter;
    int duration;
};

/**
 * Main class that coordinates the loading and displaying of pictures.
 * Some of the work is done in a separate thread so we don't block user
 * events.  The basic flow for loading and displaying images is as
 * follows:
 *
 * 1.  When the screensaver starts it paints a blank picture.
 * 2.  Traverse user specified pictures directory looking for images
 *     that we can display.
 * 3.  Load the image in the main thread.
 * 4.  Start a worker thread to rescale the image.
 * 5.  Poll the worker thread to see if its finished while processing.
 * 6.  When it has finished set the current display image to the rescaled
 *     image from the worker thread.
 * 7.  Send a paint event.
 * 8.  The paint() function renders the current image.
 * 9.  Start loading the next picture and resize it in a separate thread
 *     like before (Steps 2-5).
 * 10. Wait for a user defined timeout before displaying the picture.
 *     (Steps 6-8)
 * 11. Go back to step 9 and repeat.
 *
 * Steps 5 and 8 process Windows events. paint() can be called at these
 * steps if Windows sends our window a WM_PAINT event.
 *
 * Both the fullscreen mode and the small control panel preview mode
 * use the same code.
 *
 * This class also handles the setup of the config dialog.
 */
class PhotoViewerScreenSaver : public Platform::ScreenSaverApplication
{
public:
    PhotoViewerScreenSaver(const wstring &cmd_line)
      : Platform::ScreenSaverApplication(L"PhotoViewer Screensaver", cmd_line),
        settings(getRegistrySettings()),
        loaded_image(NULL),
        myPictures(NULL),
        textRenderer(L"Microsoft Sans Serif"),  // Safe font, GDI will pick a suitable font for unicode chars
        directoryEmpty(false),
        resizeImageThread(NULL),
        state(SHOW_NEXT_IMAGE)
    {
    }

    virtual ~PhotoViewerScreenSaver()
    {
        // Kill any timers left so none of the timer callbacks get called.
        if (windows.size()) {
            killTimers();
        }

        // Close all the windows
        for (vector<Window*>::iterator i = windows.begin(); i != windows.end(); ++i) {
            delete (*i);
        }

        if (resizeImageThread) {
            resizeImageThread->kill();
        }

        if (loaded_image != NULL) {
            FreeImage_Unload(loaded_image);
        }

        // Delete display bitmaps for each window
        for (map<Window*, FIBITMAP*>::iterator i = display_images.begin(); i != display_images.end(); ++i) {
            FreeImage_Unload((*i).second);
        }
    }


private:
    bool startScreenSaver(HWND parent_window)
    {
        bool window_opened = false;

        if (parent_window == NULL) {
            // Fullscreen mode
            // Multimonitor support
            vector<HMONITOR> monitors = getMonitors();
            if (monitors.size() == 0) {
                // If the monitor function wasn't available
                Window *window = createWindow();
                window_opened = window->openFullScreen();
                windows.push_back(window);
            }
            else {
                for (vector<HMONITOR>::iterator i = monitors.begin();
                        i != monitors.end(); ++i) {
                    Window *window = createWindow();
                    if (window->openFullScreen(*i)) {
                        window_opened = true;
                        windows.push_back(window);
                    }
                    else {
                        delete window;
                    }
                }
            }
        }
        else
        {
            // Windowed mode
            Window *window = createWindow();
            window_opened = window->open(parent_window);
            windows.push_back(window);
        }

        if (!window_opened) {
            Log::print(L"Failed to create window. Exiting...");
            return false;
        }

        resetPictureList();

        current_window = windows.begin();
        showNextImageTimerId = currentWindow()->getNewTimerId();
        checkIfImageIsResizedTimerId = currentWindow()->getNewTimerId();
        loadNextImage();
        return processEvents();
    }

    Window *createWindow()
    {
        // Create double buffered window
        return new Platform::Window(L"Photo Viewer", *this, true);
    }

    bool paint(Window *window)
    {
        if (directoryEmpty) {
            PAINTSTRUCT paintInfo;
            HDC hdc = window->beginPaint(&paintInfo);
            window->clear(hdc);
            wstring text = L"No images found in:\n" + myPictures->getBaseDirectory();
            textRenderer.render(hdc, text);
            window->endPaint(&paintInfo);
            return true;
        }

        if (display_images.find(window) == display_images.end()) {
            return false;
        }

        pasteBitmapToWindow(window, display_images[window]);
        return true;
    }

    void timer(Window *window, int timerId)
    {
        if (timerId == showNextImageTimerId && state == SHOW_NEXT_IMAGE) {
            if (loaded_image) {
                displayResizedImage();
            }
            loadNextImage();
        }

        if (timerId == checkIfImageIsResizedTimerId && state == RESIZING_IMAGE) {
            if (!resizeImageThread->isThreadRunning()) {
                startShowNextImageTimer();
            }
        }
    }

    void loadNextImage()
    {
        // Reset the picture list if the user changes the base path
        if (myPictures->getBaseDirectory() != settings.getBasePicturesPath()) {
            resetPictureList();
        }

        FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;

        FreeImageIO io;
        io.read_proc  = FreeImageReadProc;
        io.write_proc = NULL;
        io.seek_proc  = FreeImageSeekProc;
        io.tell_proc  = FreeImageTellProc;

        wstring picture_path;
        bool reset_once = false;
        directoryEmpty = false;
        while (image_format == FIF_UNKNOWN) {
            picture_path = myPictures->getNextFilePath();
            // Check if any pictures
            if (picture_path.empty()) {
                // If we can't find any images
                if (reset_once) {
                    // Draw the can't find any image files message
                    // setting a flag and forcing a redraw.  All drawing
                    // is done in the paint method in case we have to redraw
                    // when another window pops in front of us.
                    directoryEmpty = true;
                    currentWindow()->sendPaintEvent();
                    // Call this function again after the user specified wait
                    // to check for any new picture files.
                    currentWindow()->startTimer(showNextImageTimerId, settings.getNextPictureWait());
                    return;
                }

                // Restart listing the files if we havent done so once before
                myPictures->reset();
                reset_once = true;
                continue;
            }

            wstring::size_type i = picture_path.find_last_of(L".");
            if (i != wstring::npos) {
                wstring extension(picture_path, i, picture_path.size());
                string c_extension = to_string(extension);
                image_format = FreeImage_GetFIFFromFilename(c_extension.c_str());
            }
        }

        if (image_format != FIF_UNKNOWN) {
            FILE *file = _wfopen(picture_path.c_str(), L"rb");
            if (file != NULL) {
                loaded_image = FreeImage_LoadFromHandle(image_format, &io, (fi_handle)file, 0);
                fclose(file);

                if (loaded_image == NULL) {
                    (*current_window)->startTimer(showNextImageTimerId, 0);
                    return;
                }

                // Remember last picture, so the screen saver doesnt start
                // from the beginning the next time its run
                settings.setLastPicturePath(picture_path);

                if (!settings.showSmallImages()) {
                    int width = FreeImage_GetWidth(loaded_image);
                    int height = FreeImage_GetHeight(loaded_image);
                    if (width  < settings.getMinPictureWidth() ||
                            height < settings.getMinPictureHeight()) {
                        FreeImage_Unload(loaded_image);
                        loaded_image = NULL;
                        (*current_window)->startTimer(showNextImageTimerId, 0);
                        return;
                    }
                }

                Log::print(L"Resizing image " + picture_path + L"...");
                startResizeImageThread(loaded_image, (*current_window)->getWidth(), (*current_window)->getHeight());
            }
        }
    }

    void startResizeImageThread(FIBITMAP *image, int new_width, int new_height)
    {
        delete resizeImageThread;
        resizeImageThread = new ResizeImageThread(image,
                new_width,
                new_height,
                settings.getRescaleFilterType());
        resizeImageThread->start();
        currentWindow()->startTimer(checkIfImageIsResizedTimerId, 100);
        state = RESIZING_IMAGE;
    }

    void startShowNextImageTimer()
    {
        int wait = 0;
        if (getDisplayImageForCurrentWindow()) {
            wait = settings.getNextPictureWait() -
                   resizeImageThread->getDuration();

            if (wait < 0) {
                wait = 0;
            }

            Log::printf(L"Waiting %d ms before showing image...", wait);
        }
        currentWindow()->startTimer(showNextImageTimerId, wait);
        state = SHOW_NEXT_IMAGE;
    }

    void displayResizedImage()
    {
        FreeImage_Unload(loaded_image);
        loaded_image = NULL;

        if (getDisplayImageForCurrentWindow()) {
            FreeImage_Unload(getDisplayImageForCurrentWindow());
        }

        display_images[currentWindow()] = resizeImageThread->getRisizedImage();
        currentWindow()->sendPaintEvent();

        killTimers();

        // Move to next window if multiple windows were created
        current_window++;
        if (current_window == windows.end()) {
            current_window = windows.begin();
        }
    }

    bool showConfigureDialog(HWND parent)
    {
        ConfigDialog configDialog(parent, settings);
        return configDialog.exec();
    }

    void resetPictureList()
    {
        delete myPictures;
        myPictures = new FileList(settings.getBasePicturesPath());
        myPictures->setCurrentFile(settings.getLastPicturePath());
    }

    void pasteBitmapToWindow(Window *window, FIBITMAP *image)
    {
        int image_width = FreeImage_GetWidth(image);
        int image_height = FreeImage_GetHeight(image);

        int x_offset = 0;
        int y_offset = 0;

        if (image_width == window->getWidth()) {
            y_offset = (window->getHeight() - image_height) / 2;
        }
        else {
            x_offset = (window->getWidth() - image_width) / 2;
        }

        // FIXME: Convert to bitmap of same depth as desktop
        PAINTSTRUCT paintInfo;
        HDC hdc = window->beginPaint(&paintInfo);
        SetDIBitsToDevice(hdc,
                          // x-y-coord of destination upper-left corner
                          x_offset,
                          y_offset,
                          // width-height of destination rectangle
                          image_width,
                          image_height,
                          // x-y-coord of source upper-left corner
                          0, 0,
                          0,                           // Scanline offset
                          image_height,                // Number of scan lines
                          FreeImage_GetBits(image),    // bitmap bits
                          FreeImage_GetInfo(image),    // bitmap data
                          DIB_RGB_COLORS               // usage options
                         );

        // Clear parts of image we didnt just draw to
        if (image_width == window->getWidth()) {
            PatBlt(hdc,
                   x_offset, y_offset + image_height,
                   window->getWidth(), window->getHeight(),
                   BLACKNESS);
            PatBlt(hdc, 0, 0, image_width, y_offset, BLACKNESS);
        }
        else {
            PatBlt(hdc,
                   x_offset + image_width, y_offset,
                   window->getWidth(), window->getHeight(),
                   BLACKNESS);
            PatBlt(hdc, 0, 0, x_offset, image_height, BLACKNESS);
        }

        window->endPaint(&paintInfo);
    }

    void killTimers()
    {
        currentWindow()->stopTimer(showNextImageTimerId);
        currentWindow()->stopTimer(checkIfImageIsResizedTimerId);
    }

    Window *currentWindow() { return *current_window; }

    FIBITMAP *getDisplayImageForCurrentWindow()
    {
        if (display_images.find(currentWindow()) == display_images.end()) {
            return NULL;
        }

        return display_images[currentWindow()];
    }

    // Convert the given wide string into an 8-bit string
    // using the locale settings.
    std::string to_string(const std::wstring& source)
    {
        std::string result(source.size(), wchar_t(0));
        typedef std::ctype<wchar_t> ctype_t;
        const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
        ct.narrow(source.data(), source.data() + source.size(), '@', &(*result.begin()));
        return result;
    }

    // Freeimage function prototypes needed to allow reading files with unicode file names
    static size_t _stdcall FreeImageReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
    {
        return fread(buffer, size, count, (FILE *)handle);
    }

    static int _stdcall FreeImageSeekProc(fi_handle handle, long offset, int origin)
    {
        return fseek((FILE *)handle, offset, origin);
    }

    static long _stdcall FreeImageTellProc(fi_handle handle)
    {
        return ftell((FILE *)handle);
    }

    // Hold the state, because I don't bother killing the timers.
    enum State {
        SHOW_NEXT_IMAGE,
        RESIZING_IMAGE,
    };
    State state;

    FileList *myPictures;
    vector<Window *> windows;
    vector<Window *>::iterator current_window;
    map<Window *, FIBITMAP *> display_images;
    FIBITMAP *loaded_image;
    int showNextImageTimerId;
    int checkIfImageIsResizedTimerId;
    bool directoryEmpty;
    PhotoViewerConfig settings;
    TextRenderer textRenderer;
    ResizeImageThread *resizeImageThread;
};

int APIENTRY _tWinMain(HINSTANCE instance,
                       HINSTANCE prev_instance,
                       _TCHAR*   cmd_line,
                       int       cmd_show)
{
    Log::print(L"PhotoViewer by watsonmw@gmail.com");

    PhotoViewerScreenSaver screen_saver(cmd_line);
    screen_saver.run();

    return 0;
}