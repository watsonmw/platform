#ifndef PLATFORM_FILEINFO_DOT_H
#define PLATFORM_FILEINFO_DOT_H

#include <string>

/**
 * Class for get file attributes.
 */
namespace Platform
{
    class FileInfo
    {
    public:
        FileInfo(const std::wstring &fileName);
        ~FileInfo();

        /**
         * Checks if exists
         */
        bool exists();

        /**
         * Checks if readable
         */
        bool isReadable();

        /**
         * Checks if writable
         */
        bool isWritable();

        /**
         * Checks if this object repesents a file or a link to a valid file
         */
        bool isFile();

        /**
         * Checks if this object repesents a valid directory
         */
        bool isDirectory();


    private:
        std::wstring _fileName;
        bool _bDirectory;
        bool _bPathExists;
    };
}

#endif
