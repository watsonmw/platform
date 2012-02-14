#ifndef _PLATFORM_REGISTRYSETTINGS_H_INCLUDED_
#define _PLATFORM_REGISTRYSETTINGS_H_INCLUDED_

#include <string>
#include <windows.h>

/**
* Read and set values in the registy for the given
* application name.
*/
namespace Platform
{
    class RegistrySettings
    {
    public:
        RegistrySettings(const std::wstring &application_name);
        ~RegistrySettings();

        /**
        * Functions for getting and setting ints, bools and strings
        */
        std::wstring getString(const std::wstring &);
        bool getBool(const std::wstring &, bool defaultValue = false);
        int getInt(const std::wstring &);

        void set(const std::wstring &setting,
                 const std::wstring &value);

        void set(const std::wstring &setting,
                 bool value);

        void set(const std::wstring &setting,
                 int value);

        /**
         * Check if a setting exists in the registry
         */
        bool exists(const std::wstring &setting);

        /**
        * Functions for operating on a kist of values
        */
        int count_list(const std::wstring &list);
        std::wstring get_list_value(const std::wstring &list, int i);
        void add_list_value(const std::wstring &list,
                            const std::wstring &value);

    protected:
        LONG openApplicationKey(HKEY &application_key);

        std::wstring application_name;
    };
};

#endif