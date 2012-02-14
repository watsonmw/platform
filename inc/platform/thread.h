#ifndef _PLATFORM_THREAD_INCLUDED_H_
#define _PLATFORM_THREAD_INCLUDED_H_

/**
 * Thin wrapper class around win32 threads.  To create your own thread,
 * subclass this class and implement the run function
 */
namespace Platform
{
    class Thread
    {
    public:
        Thread();
        virtual ~Thread();

        /**
         * Call this function to begin executing the thread
         */
        void start();

        /**
         * Return true if the thread is still running.
         */
        bool isThreadRunning();

        /**
         * Forceably kill the thread.  Any memory, files, critical sections, etc
         * that the thread has open won't be cleaned up.
         */
        void kill();


    protected:
        /**
         * Implement this function to do the actual work in the
         * new thread.  Called straight after start() is called.
         */
        virtual void run() = 0;


    private:
        static DWORD WINAPI runThreadInMemberFunction(LPVOID lpParam);

        HANDLE _handle;
        DWORD _threadId;
    };
}

#endif