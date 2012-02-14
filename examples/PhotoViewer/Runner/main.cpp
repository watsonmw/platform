#include "stdafx.h"

#include <string>

using namespace std;

wstring getApplicationDirectory()
{
    wstring key_name = L"Software\\WatsonWare\\PhotoViewer Screensaver";

    HKEY app_directory;
    LONG error_code = RegOpenKey(HKEY_LOCAL_MACHINE,
                                 key_name.c_str(),
                                 &app_directory);

    DWORD data_type;
    const int BUFFER_SIZE = 1024;
    TCHAR data[BUFFER_SIZE];
    DWORD data_size = BUFFER_SIZE;
    error_code = RegQueryValueEx(app_directory,
                                 L"",
                                 0,
                                 &data_type,
                                 (BYTE*)data,
                                 &data_size);

    if (error_code != ERROR_SUCCESS ||
            (data_type != REG_SZ && data_type != REG_EXPAND_SZ))
        return L"";

    return wstring(data);
}

int APIENTRY _tWinMain(HINSTANCE instance,
                       HINSTANCE prev_instance,
                       _TCHAR*   cmd_line,
                       int       cmd_show)
{
    wstring file = L"PhotoViewer.scr";
    wstring directory = getApplicationDirectory();
    wstring full_path = directory + L"\\" + file;

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

	PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    wstring full_cmd_line = file + L" " + (TCHAR*)cmd_line;
    CreateProcess(full_path.c_str(), (wchar_t *)full_cmd_line.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    return 0;
}