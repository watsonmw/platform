#ifndef _PLATFORM_LOG_DOT_H_INCLUDED_
#define _PLATFORM_LOG_DOT_H_INCLUDED_

#include <stdio.h>
#include <string>

namespace Platform
{
    /**
     * Basic logging to a file.
     * The format/code page of the file depends on what strings you
     * give the logging functions.  All strings are logged "as is" to
     * the file.
     *
     * NOTE: Don't mix charactors sets when logging as likely this
     * will cause problems with the file reader.
     */ 
    class Log
    {
    public:
        virtual ~Log();

        /**
         * Static functions for outputing text to a log file.
         */
        static void print(const char *);
        static void print(const std::string &);
        static void printf(const char *, ...);
        static void print(const wchar_t *);
        static void print(const std::wstring &);
        static void printf(const wchar_t *, ...);

        /**
         * If there is a win32 error available from GetLastError()
         * then print it to the logs.
         * Returns true if an error is set, false otherwise.
         */
        static bool logWin32Error();

        /**
         * If there is a OpenGL error available from glGetError()
         * then print it to the logs.
         * Returns true if an error is set, false otherwise.
         */
        static bool logOpenGLError();

    private:
        static Log &getInstance();

        void doPrint(const char *);
        void doPrint(const std::string &);
        void doPrintf(const char *, ...);
        void doPrint(const wchar_t *);
        void doPrint(const std::wstring &);
        void doPrintf(const wchar_t *, ...);

        bool doLogWin32Error();
        bool doLogOpenGLError();

        Log();

        FILE* _file;
    };
};

#endif