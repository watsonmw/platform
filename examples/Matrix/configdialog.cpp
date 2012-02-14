#include "StdAfx.h"
#include "configdialog.h"
#include "commctrl.h"
#include "resource.h"

#include "platform/log.h"
#include "platform/settings.h"

using namespace Platform;


BOOL CALLBACK ConfigDialog::DialogProc(HWND hWnd,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_OK:
            EndDialog(hWnd, IDC_OK);
            return TRUE;
        case IDC_CANCEL:
            EndDialog(hWnd, IDC_CANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}


ConfigDialog::ConfigDialog(HWND parent)
        : parent(parent)
{
    INITCOMMONCONTROLSEX commonControls;
    commonControls.dwSize = sizeof(INITCOMMONCONTROLSEX);
    commonControls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&commonControls);
}


ConfigDialog::~ConfigDialog()
{
}


void ConfigDialog::exec()
{
    HINSTANCE currentModule = GetModuleHandle(NULL);
    INT_PTR returnValue = DialogBox(currentModule,
                                    TEXT("MATRIXSETUP"),
                                    parent,
                                    (DLGPROC)ConfigDialog::DialogProc);

    if (returnValue == -1)
        Log::logWin32Error();

    switch (returnValue) {
    case IDC_OK:
    case IDC_CANCEL:
        break;
    }
}
