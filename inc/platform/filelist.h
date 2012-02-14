#ifndef _PLATFORM_FILELIST_H_INCLUDED_
#define _PLATFORM_FILELIST_H_INCLUDED_

#include "platform/directory.h"
#include <string>
#include <stack>

/**
 * Recurively list all files in a directory tree
 */
class FileList
{
public:
    FileList(const std::wstring &basePath);
    virtual ~FileList();

    /**
     *  Returns an empty string ("") when all files in current directory have been
     *  returned.
     */
    std::wstring getNextFilePath();

	/**
	 * Reset cursor
	 */
    void reset();

    /**
	 * Debug function to list all files in the directory tree
     */
    void printAllFiles();

	/**
	 * Skip to specific file in the directory listing, and continue
	 * listing from there.
	 */
    bool setCurrentFile(const std::wstring &file);

	/**
	 * Return the base directory that we are listing.
	 */
    std::wstring getBaseDirectory() const;

private:
    std::wstring getFileInSubDirectory();

    Directory _baseDirectory;
    std::stack<Directory *> _directories;
};

#endif
