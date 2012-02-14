#include "StdAfx.h"

#include "configdialog.h"

#include <commctrl.h>
#include <objbase.h>
#include <shlobj.h>

#include "resource.h"
#include "photoviewerconfig.h"
#include "platform/win32.h"
#include "platform/log.h"
#include "platform/fileinfo.h"
#include "platform/settings.h"
#include "platform/registrysettings.h"

static ConfigDialog *config_dialog = NULL;

using namespace std;
using namespace Platform;

// Code ripped from Microsoft's SHBrowseForFolder example.
// (Slightly modified so it actually compiles!)
// Apparently some versions of SHBrowseForFolder return links rather
// than full paths so we have to do some work to convert it.
// I say apparently because Win2k sp3 doesn't give you the option
// to click on links.  And according to the docs WinXP always returns
// the path the link points to.

// Retrieves the UIObject interface for the specified full PIDL
STDAPI SHGetUIObjectFromFullPIDL(LPITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
{
    LPCITEMIDLIST pidlChild;
    IShellFolder* psf;

    *ppv = NULL;

    HRESULT hr = SHBindToParent(pidl, IID_IShellFolder, (void**)(&psf), &pidlChild);
    if (SUCCEEDED(hr)) {
        hr = psf->GetUIObjectOf(hwnd, 1, &pidlChild, riid, NULL, ppv);
        psf->Release();
    }
    return hr;
}

// Get the target PIDL for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, etc.
STDAPI SHGetTargetFolderIDList(LPITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl)
{
    IShellLink *psl;

    *ppidl = NULL;

    HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, IID_IShellLink, (void**)(&psl));

    if (SUCCEEDED(hr)) {
        hr = psl->GetIDList(ppidl);
        psl->Release();
    }

    // It's not a folder shortcut so get the PIDL normally.
    if (FAILED(hr)) {
        *ppidl = pidlFolder;
        return S_OK;
    }

    return hr;
}

// Get the target folder for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, the My Documents folder, etc.
STDAPI SHGetTargetFolderPath(LPITEMIDLIST pidlFolder, LPWSTR pszPath)
{
    LPITEMIDLIST pidlTarget;

    *pszPath = 0;

    HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);

    if (SUCCEEDED(hr)) {
        SHGetPathFromIDListW(pidlTarget, pszPath);   // Make sure it is a path
        CoTaskMemFree(pidlTarget);
    }

    return *pszPath ? S_OK : E_FAIL;
}

ConfigDialog::ConfigDialog(HWND parent,
                           PhotoViewerConfig &settings)

  : parent(parent),
    settings(settings)
{
    INITCOMMONCONTROLSEX commonControls;
    commonControls.dwSize = sizeof(INITCOMMONCONTROLSEX);
    commonControls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&commonControls);
}

ConfigDialog::~ConfigDialog()
{
}

BOOL ConfigDialog::dialogEvent(HWND hWnd,
                               UINT message,
                               WPARAM wParam,
                               LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        // Set the spinbox range and its buddy edit control
        SendDlgItemMessage(hWnd, IDC_WAIT_SPIN, UDM_SETRANGE, 0,
                           (LPARAM)MAKELONG((short)9999, (short)1));
        SendDlgItemMessage(hWnd, IDC_WAIT_SPIN, UDM_SETBUDDY,
                           (WPARAM)GetDlgItem(hWnd, IDC_WAIT_NUM), 0);

        // Populate the picture path combo box with saved values
        for (int i = 0; i < settings.getNumRememberedPicturePaths(); i++) {
            std::wstring path = settings.getRememberedPicturePath(i);
            SendDlgItemMessage(hWnd, IDC_PICTURES_PATH, CB_ADDSTRING, 0,
                               (LPARAM)path.c_str());
        }

        updateControls(hWnd,
                       settings.getBasePicturesPath(),
                       settings.getNextPictureWait(),
                       settings.showSmallImages());

        return TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, IDC_CANCEL);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_OK:
            {
                // Pictures Folder
                TCHAR buffer[MAX_PATH+1];
                int text_lenght = GetDlgItemText(hWnd, IDC_PICTURES_PATH,
                                                    buffer, MAX_PATH);
                wstring base_path(buffer, text_lenght);

                // Check that the directory exists before remembering the user settings.
                FileInfo file_info(base_path);
                if (!file_info.exists() || !file_info.isDirectory()) {
                    wstring message = L"Unable to find directory '" + base_path + L"'.\n\n"
                                        L"Please change the selected directory and try again.";
                    MessageBox(hWnd, message.c_str(), L"PhotoViewer", 0);
                    return TRUE;
                }

                settings.setBasePicturesPath(base_path);

                // Reset the last picture, so we start from the first one
                // in the base path
                settings.setLastPicturePath(L"");

                // Wait
                int wait = (int)GetDlgItemInt(hWnd, IDC_WAIT_NUM, NULL, FALSE);
                if (wait < 1) {
                    wait = 1;
                }
                settings.setNextPictureWait(wait * 1000);

                // For short wait times just use a box or bilinear filter as they're
                // quicker.  We don't always use these because they don't look as good
                // as the Catmull Rom filter.
                if (wait == 1) {
                    settings.setRescaleFilterType(FILTER_BOX);
                }
                else if (wait <= 3) {
                    settings.setRescaleFilterType(FILTER_BILINEAR);
                }
                else {
                    settings.setRescaleFilterType(FILTER_CATMULLROM);
                }

                // Ignore Small image
                if (SendDlgItemMessage(hWnd, IDC_SKIP_SMALL_IMAGES, BM_GETCHECK, 0, 0) ==
                        BST_CHECKED) {
                    settings.setShowSmallImages(false);
                }
                else {
                    settings.setShowSmallImages(true);
                }

                EndDialog(hWnd, IDC_OK);
            }
            return TRUE;
        case IDC_CANCEL:
            EndDialog(hWnd, IDC_CANCEL);
            return TRUE;
        case IDC_BROWSE:
            {
                BROWSEINFO browse_info;
                browse_info.hwndOwner = hWnd;
                browse_info.pidlRoot = NULL;
                browse_info.lpszTitle = L"Select folder containing images.";
                browse_info.pszDisplayName = NULL;
                browse_info.ulFlags =  BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
                browse_info.lpfn = NULL;
                browse_info.lParam = 0;
                browse_info.iImage = 0;
                LPITEMIDLIST itemIDList;
                if ((itemIDList = SHBrowseForFolder(&browse_info)) != NULL) {
                    TCHAR buffer[MAX_PATH+1];
                    SHGetTargetFolderPath(itemIDList, buffer);
                    if (wstring(buffer).length() != 0)
                        SetDlgItemText(hWnd, IDC_PICTURES_PATH, buffer);
                }
            }
            return TRUE;
        case IDC_DEFAULT:
            settings.resetLastPicturePath();
            updateControls(hWnd,
                            settings.getDefaultBasePicturesPath(),
                            settings.getDefaultNextPictureWait(),
                            settings.showSmallImagesDefault());
            break;
        case IDC_ABOUT:
            MessageBox(hWnd, L"Screensaver for viewing a collection of images.\n\n"
                        L"Developed and maintained by watsonmw@gmail.com\n\n"
                        L"Some Rights Reserved.\n"
                        L"http://creativecommons.org/licenses/by-nc-sa/3.0/\n\n"
                        L"This software uses the FreeImage open source image library.\n"
                        L"FreeImage is used under the §6.1 FIPL license.\n"
                        L"http://freeimage.sourceforge.net/\n",
                        L"About PhotoViewer", 0);
            break;
        }
        break;
    }
    return FALSE;
}


void ConfigDialog::setCheckBox(HWND hWnd, int checkBox, bool checked)
{
    if (checked) {
        SendDlgItemMessage(hWnd, checkBox, BM_SETCHECK, BST_CHECKED, 0);
    }
    else {
        SendDlgItemMessage(hWnd, checkBox, BM_SETCHECK, BST_UNCHECKED, 0);
    }
}

void ConfigDialog::updateControls(HWND hWnd,
                                  const std::wstring &pictures_path,
                                  int next_picture_wait,
                                  bool show_small_images)
{
    SetDlgItemText(hWnd, IDC_PICTURES_PATH, pictures_path.c_str());
    SetDlgItemInt(hWnd, IDC_WAIT_NUM, next_picture_wait / 1000, FALSE);
    setCheckBox(hWnd, IDC_SKIP_SMALL_IMAGES, !show_small_images);
}

BOOL CALLBACK ConfigDialog::dialogProc(HWND hWnd,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
    return config_dialog->dialogEvent(hWnd, message, wParam, lParam);
}

bool ConfigDialog::exec()
{
    CoInitialize(NULL);
    HINSTANCE currentModule = GetModuleHandle(NULL);
    config_dialog = this;
    INT_PTR returnValue = DialogBox(currentModule,
                                    TEXT("PHOTOVIEWERSETUP"),
                                    parent,
                                    (DLGPROC)ConfigDialog::dialogProc);

    if (returnValue == -1) {
        Log::logWin32Error();
        return false;
    }

    switch (returnValue) {
    case IDC_OK:
    case IDC_CANCEL:
        break;
    }

    return true;
}
