#include "stdafx.h"

#include "platform/fileinfo.h"

Platform::FileInfo::FileInfo(const std::wstring &fileName)
    : _fileName(fileName)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesEx(_fileName.c_str(),
                            GetFileExInfoStandard,
                            &fileInfo)) {
        _bPathExists = true;

        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            _bDirectory = true;
		}
		else {
            _bDirectory = false;
		}
    }
    else {
        _bPathExists = false;
        _bDirectory = false;
    }
}

Platform::FileInfo::~FileInfo()
{
}

bool Platform::FileInfo::exists()
{
    return _bPathExists;
}

bool Platform::FileInfo::isReadable()
{
    HANDLE hFile = CreateFile(_fileName.c_str(),
                              GENERIC_READ,
                              FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                              0,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              0);

    if (INVALID_HANDLE_VALUE == hFile)
        return false;

    CloseHandle(hFile);

    return true;
}

bool Platform::FileInfo::isWritable()
{
    if (_bDirectory) {
        //
        // Check if we can add/remove files from the directory.
        //
        TCHAR tmpFileName[MAX_PATH];

        if (GetTempFileName(_fileName.c_str(), L"tmp", 0, tmpFileName)) {
            HANDLE hFile = CreateFile(tmpFileName,
                                      GENERIC_WRITE,
                                      FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      0,
                                      OPEN_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL,
                                      0);

            if (INVALID_HANDLE_VALUE == hFile) {
                return false;
			}

            CloseHandle(hFile);
            DeleteFile(tmpFileName);

            return true;
        }

        return false;
    }
    else {
        if (_bPathExists) {
            HANDLE hFile = CreateFile(_fileName.c_str(),
                                      GENERIC_WRITE,
                                      FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      0,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      0);

            if (INVALID_HANDLE_VALUE == hFile) {
                return false;
			}

            CloseHandle(hFile);
            return true;
        }
    }

    return false;
}

bool Platform::FileInfo::isFile()
{
    return !_bDirectory;
}

bool Platform::FileInfo::isDirectory()
{
    return _bDirectory;
}
