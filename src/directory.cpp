#include "stdafx.h"
#include "platform/directory.h"
#include "platform/log.h"

using namespace std;
using namespace Platform;

Directory::Directory(const wstring &path)
   : _path(path),
     _bFoundFirstFile(false)
{
}

Directory::~Directory()
{
    reset();
}

wstring Directory::getNextFileName()
{
    do {
        if (!readNextFileInfo()) {
            return L"";
        }
    } while (_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

    return getFullPath(_fileData.cFileName);
}

void Directory::reset()
{
    if (_bFoundFirstFile) {
        FindClose(_searchHandle);
    }

    _bFoundFirstFile = false;
}

Directory *Directory::getNextSubDirectory()
{
    while (true) {
        if (!readNextFileInfo()) {
            return NULL;
        }

        if (_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (wcscmp(_fileData.cFileName, L".") == 0) {
                continue;
            }

            if (wcscmp(_fileData.cFileName, L"..") == 0) {
                continue;
            }

            break;
        }
    }

    return new Directory(getFullPath(_fileData.cFileName));
}

bool Directory::getSubDirectories(vector<Directory> &subDirectories)
{
    WIN32_FIND_DATA fileData;

    wstring searchDirectory = _path + L"/*";
    HANDLE searchHandle = FindFirstFile(searchDirectory.c_str(), &fileData);
    if (searchHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Note: We ignore the first directory its just '.'
    // The second will be '..' so ignore that too
    FindNextFile(searchHandle, &fileData);

    while (FindNextFile(searchHandle, &fileData)) {
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            subDirectories.push_back(Directory(getFullPath(fileData.cFileName)));
        }
    }

    FindClose(searchHandle);
    return true;
}

void Directory::listFilesRecursivly()
{
    listFiles();

    Directory *directory = getNextSubDirectory();
    while (directory != NULL) {
        directory->listFilesRecursivly();
        delete directory;

        directory = getNextSubDirectory();
    }
}

wstring Directory::getPath() const
{
    return _path;
}

void Directory::listFiles()
{
    // Save current file for restoration later
    HANDLE savedSearchHandle = _searchHandle;
    bool savedFoundFirstFile = _bFoundFirstFile;

    _bFoundFirstFile = false;

    wstring fileName;
    while (true) {
        fileName = getNextFileName();
        if (fileName.empty()) {
            break;
        }
        Log::print(fileName);
    }

    // Restore the current file
    _searchHandle = savedSearchHandle;
    _bFoundFirstFile = savedFoundFirstFile;
}

bool Directory::readNextFileInfo()
{
    if (!_bFoundFirstFile) {
        wstring searchDirectory = _path + L"/*";
        _searchHandle = FindFirstFile(searchDirectory.c_str(), &_fileData);
        if (_searchHandle == INVALID_HANDLE_VALUE) {
            return false;
        }

        _bFoundFirstFile = true;
    }
    else {
        if (!FindNextFile(_searchHandle, &_fileData)) {
            if (GetLastError() == ERROR_NO_MORE_FILES) {
                return false;
            }
        }
    }
    return true;
}

wstring Directory::getFullPath(const wstring &file)
{
    return _path + L"\\" + file;
}

bool Directory::setCurrentFile(const wstring &file)
{
    while (readNextFileInfo()) {
        if (!(_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (file == _fileData.cFileName) {
                return true;
            }
        }
    }

    reset();
    return false;
}

bool Directory::setCurrentSubDirectory(const wstring &subDirectory)
{
    while (readNextFileInfo()) {
        if ((_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (subDirectory == _fileData.cFileName) {
                return true;
            }
        }
    }

    reset();
    return false;
}
