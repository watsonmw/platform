#ifndef CONIGDIALOG_H_INCLUDED
#define CONIGDIALOG_H_INCLUDED

class ConfigDialog
{
public:
    ConfigDialog(HWND parent = NULL);
    virtual ~ConfigDialog();

    void exec();

private:
    HWND parent;

    static BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
};

#endif