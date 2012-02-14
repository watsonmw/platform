#ifndef _PLATFORM_APPLICATION_H_INCLUDED_
#define _PLATFORM_APPLICATION_H_INCLUDED_

#include <string>
#include "platform/registrysettings.h"
#include "platform/eventhandler.h"

namespace Platform
{
    class Application : public Platform::EventHandler
    {
    public:
        Application(const std::wstring &name,
                    const std::wstring &cmdLine);
        virtual ~Application();

        virtual bool run() = 0;

    protected:
        /**
         * Returns an accessor to our application registry settings
         */
        RegistrySettings &getRegistrySettings() {
            return _registrySettings;
        }

        /**
         * Process events until there are none left.
         * Returns false if there was a problem getting messages
         */
        bool processEvents();

        /**
         * Process events for the given number of milliseconds then return.
         * Returns false if there was a problem getting messages.
         */
        bool processEvents(int sleepTimeMilliseconds);

        /**
         * Processes any pending events and return.
         * Returns false if the app is quiting otherwise returns true
         */
        bool processPendingEvents();

    protected:
        std::wstring _name;
        std::wstring _cmdLine;
        RegistrySettings _registrySettings;
    };
};

#endif
