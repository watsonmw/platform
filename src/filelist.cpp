#include "stdafx.h"

#include "platform/filelist.h"
#include "platform/fileinfo.h"
#include "platform/filepath.h"
#include "platform/log.h"

using namespace std;
using namespace Platform;

FileList::FileList(const wstring &basePath)
  : _baseDirectory(basePath)
{
    _directories.push(&_baseDirectory);
}

FileList::~FileList()
{
    reset();
}

wstring FileList::getNextFilePath()
{
    wstring next_file_name =
        _directories.top()->getNextFileName();

    if (next_file_name.empty()) {
        _directories.top()->reset();
        return getFileInSubDirectory();
    }

    return next_file_name;
}

void FileList::printAllFiles()
{
    wstring filePath;
    while (true) {
        filePath = getNextFilePath();
        if (filePath.empty()) {
            break;
        }
        Log::print(filePath);
    }
}

void FileList::reset()
{
    while (_directories.top() != &_baseDirectory) {
        delete _directories.top();
        _directories.pop();
    }
    _baseDirectory.reset();
}


bool FileList::setCurrentFile(const wstring &current_file)
{
    wstring base_path = FilePath::cleanUpPath(_baseDirectory.getPath()) + L"\\";;
    if (current_file.find(base_path) != 0) {
        return false;
    }

    if (base_path.length() == current_file.length()) {
        return false;
    }

    vector<wstring> split_path;
    FilePath::splitFilePath(current_file.substr(base_path.length()), split_path);

    wstring current_path = base_path;
    for (vector<wstring>::iterator i = split_path.begin(); i != split_path.end(); ++i) {
        wstring name = *i;
        current_path += name + L"\\";

        FileInfo fileInfo(current_path);
        if (fileInfo.isDirectory()) {
            Directory *next_directory = new Directory(current_path);
            _directories.top()->setCurrentSubDirectory(name);
            _directories.push(next_directory);
        }
        else if (fileInfo.isFile()) {
            return _directories.top()->setCurrentFile(name);
        }
        else {
            return false;
        }
    }
    return false;
}

wstring FileList::getBaseDirectory() const
{
    return _baseDirectory.getPath();
}

wstring FileList::getFileInSubDirectory()
{
    Directory *next_directory = _directories.top()->getNextSubDirectory();

    if (next_directory) {
        _directories.push(next_directory);
        return getNextFilePath();
    }
    else {
        if (_directories.size() == 1) {
            return L"";
        }
        else {
            delete _directories.top();
            _directories.pop();

            return getFileInSubDirectory();
        }
    }
}
