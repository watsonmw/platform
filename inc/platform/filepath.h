#ifndef _PLATFORM_FILEPATH_H_INCLUDED
#define _PLATFORM_FILEPATH_H_INCLUDED

#include <vector>
#include <string>

/**
 * Helper functions for operating on file paths.
 */
namespace FilePath
{
    /**
     * Split a file name path into a list of dircetory components (and file name if there is one)
     */
    std::size_t splitFilePath(const std::wstring &path, std::vector<std::wstring> &parts);

    /**
     * Cleans up a file path so it is separated by single '\'s.
     */
    std::wstring cleanUpPath(const std::wstring &path);
};

#endif