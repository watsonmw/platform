#ifndef _PLATFORM_DIRECTORY_H_INCLUDED
#define _PLATFORM_DIRECTORY_H_INCLUDED

#include <windows.h>
#include <string>
#include <vector>

/**
 * Class for traversing a directory hieracy.
 */
class Directory
{
public:
    Directory(const std::wstring &path);
    virtual ~Directory();

    /**
     * returns the next file in the directory listing
     */
    std::wstring getNextFileName();

    /**
     * Set the current file, this effects the getNextFileName function.
     * Its provided as a way to restore state from a previous Directory
     * instance after getNextFileName has been called on that directory.
     *
     * Returns false if it can find the specified file.
     */
    bool setCurrentFile(const std::wstring &file);

    /**
     * Reset the directory listing to the start.  getNextFileName() will
     * return the first file in the directory the next time it is called.
     */
    void reset();

    /**
     * Return the next subdirectory.
     */
    Directory *getNextSubDirectory();

    /**
     * Set the current sub directory, this effects the getNextSubDirectory
     * function.
     * Its provided as a way to restore state from a previous Directory
     * instance after getNextSubDirectory has been called on that directory.
     *
     * Returns false if it can find the specified directory.
     */
    bool setCurrentSubDirectory(const std::wstring &sub_directory);

    /**
     * Returns a list of sub directories under this one
     */
    bool getSubDirectories(std::vector<Directory> &sub_directories);

    /**
     * print all files in a directory heirachy out to the log
     */
    void listFilesRecursivly();

    std::wstring getPath() const;

private:
    /**
     * print all files out to log
     */
    void listFiles();

    /**
     * Reads the next file in directory after
     */
    bool readNextFileInfo();

    std::wstring getFullPath(const std::wstring &file);

    bool _bFoundFirstFile;
    WIN32_FIND_DATA _fileData;
    std::wstring _path;
    HANDLE _searchHandle;
};

#endif
