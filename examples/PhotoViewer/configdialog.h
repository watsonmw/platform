#ifndef CONIGDIALOG_H_INCLUDED
#define CONIGDIALOG_H_INCLUDED

#include <string>

class PhotoViewerConfig;

class ConfigDialog
{
public:
    ConfigDialog(HWND parent, PhotoViewerConfig &settings);
    virtual ~ConfigDialog();

    bool exec();

private:
    HWND parent;
    PhotoViewerConfig &settings;

    BOOL dialogEvent(HWND, UINT, WPARAM, LPARAM);
    void updateControls(HWND hWnd,
                        const std::wstring &pictures_path,
                        int next_picture_wait,
                        bool show_small_images);
    static void setCheckBox(HWND hWnd, int checkBox, bool checked);
    static BOOL CALLBACK dialogProc(HWND, UINT, WPARAM, LPARAM);
};

#endif