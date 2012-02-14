#ifndef _PLATFORM_SETTINGS_H_INCLUDED_
#define _PLATFORM_SETTINGS_H_INCLUDED_

#include <string>
#include <map>

namespace Platform
{
    class Settings
    {
    public:
        static Settings &getInstance();
        ~Settings();

        std::wstring getString(const std::wstring &);
        std::wstring getPath(const std::wstring &);
        bool getBool(const std::wstring &);
        int getInt(const std::wstring &);

        void set(const std::wstring &setting,
                 const std::wstring &value);

        bool write();


    protected:
        typedef std::map<std::wstring, std::wstring> SettingsMap;
        typedef SettingsMap::iterator SettingsMapIterator;

        bool loadSettings(const std::wstring &file_name,
                          SettingsMap &settings);

        bool writeSettings(const std::wstring &file_name,
                           SettingsMap &settings);

        std::wstring getCustomSettingFilePath();

        Settings();

        SettingsMap defaultSettings;
        SettingsMap customSettings;
    };
};

#endif