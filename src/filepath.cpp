#include "stdafx.h"

#include "platform/filepath.h"
#include "platform/log.h"

using namespace std;
using namespace Platform;

static size_t findNextPathSeparator(const wstring &path, size_t base_path_end)
{
    size_t next_forward_slash  = path.find(L"\\", base_path_end);
    size_t next_backward_slash = path.find(L"/", base_path_end);
    size_t separator_pos = next_forward_slash;
    if (separator_pos > next_backward_slash) {
        separator_pos = next_backward_slash;
    }

    return separator_pos;
}

static bool findNextSubDirectory(const wstring &path, size_t &start, size_t &end)
{
    if (start > path.length()) {
        return false;
    }

    while (findNextPathSeparator(path, start) == start) {
        start++;
    }

    end = findNextPathSeparator(path, start);

    return true;
}

size_t FilePath::splitFilePath(const wstring &path, vector<wstring> &parts)
{
    size_t start = 0;
    size_t end = 0;
    while (findNextSubDirectory(path, start, end)) {
        wstring part_name = path.substr(start, end - start);

        if (!part_name.empty()) { // Can be empty at end of string
            parts.push_back(part_name);
        }

        start = end;
    }

    return parts.size();
}

std::wstring FilePath::cleanUpPath(const std::wstring &path)
{
    vector<wstring> parts;
    splitFilePath(path, parts);
    wstring clean_path;
    for (vector<wstring>::iterator i = parts.begin(); i != parts.end(); ++i) {
        wstring name = *i;
        clean_path += name + L"\\";
    }
    return clean_path.substr(0, clean_path.length() - 1);
}

