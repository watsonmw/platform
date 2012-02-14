#include "stdafx.h"
#include "platform/log.h"

#include "gl/gl.h"
#include "gl/glu.h"

#include <stdarg.h>
#include <stdlib.h>

Platform::Log &Platform::Log::getInstance()
{
    static Log log;
    return log;
}

Platform::Log::Log()
{
    _file = fopen("log.txt", "w");

    if (_file != NULL) {
        fclose(_file);
    }
}

Platform::Log::~Log()
{
    if (_file != NULL) {
        fclose(_file);
        _file = NULL;
    }
}

void Platform::Log::print(const char *message)
{
    getInstance().doPrint(message);
}

void Platform::Log::print(const wchar_t *message)
{
    getInstance().doPrint(message);
}

void Platform::Log::print(const std::string &message)
{
    getInstance().doPrint(message);
}

void Platform::Log::print(const std::wstring &message)
{
    getInstance().doPrint(message);
}

void Platform::Log::printf(const char *format, ...)
{
    va_list   args;
    va_start(args, format);
    int len = _vscprintf(format, args) + 1; // _vscwprintf doesn't count terminating '\0'
    char* buffer = (char*)malloc(len * sizeof(char));
    vsnprintf(buffer, len-1, format, args);

    getInstance().doPrint(buffer);

    free(buffer);
}

void Platform::Log::printf(const wchar_t *format, ...)
{
    va_list   args;
    va_start(args, format);
    int len = _vscwprintf(format, args) + 1; // _vscwprintf doesn't count terminating '\0'
    wchar_t* buffer = (wchar_t*)malloc(len * sizeof(wchar_t));
    vswprintf(buffer, len-1, format, args);

    getInstance().doPrint(buffer);

    free(buffer);
}

bool Platform::Log::logWin32Error()
{
    return getInstance().doLogWin32Error();
}

bool Platform::Log::logOpenGLError()
{
    return getInstance().doLogOpenGLError();
}

void Platform::Log::doPrint(const char *buffer)
{
    _file = fopen("log.txt", "a");
    if (_file != NULL) {
        fprintf(_file, buffer);
        fprintf(_file, "\n");
        fclose(_file);
    }
}

void Platform::Log::doPrint(const wchar_t *buffer)
{
    _file = fopen("log.txt", "a");
    if (_file != NULL) {
        fwprintf(_file, buffer);
        fwprintf(_file, L"\n");
        fclose(_file);
        _file = NULL;
    }
}

void Platform::Log::doPrint(const std::wstring &string)
{
    print(string.c_str());
}

void Platform::Log::doPrint(const std::string &string)
{
    print(string.c_str());
}

bool Platform::Log::doLogWin32Error()
{
    DWORD error = GetLastError();
    if (error == ERROR_SUCCESS) {
        return false;
    }
    LPVOID msgBuffer;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &msgBuffer,
        0,
        NULL
    );
    print((TCHAR*)msgBuffer);
    LocalFree(msgBuffer);
    return true;
}

bool Platform::Log::doLogOpenGLError()
{
    int r = glGetError();
    if (r == GL_NO_ERROR) {
        return false;
    }
    printf(L"OGL Report: %s", gluErrorStringWIN(r));
    return true;
}
