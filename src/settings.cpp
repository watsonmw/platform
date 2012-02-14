#include "stdafx.h"

#include "platform/settings.h"
#include "platform/apputils.h"
#include "platform/log.h"

#include <fstream>
#include <string>

#include <cstdlib>

using namespace std;

namespace Platform
{
    Settings::Settings()
    {
        wstring default_file = AppUtils::getBasePath() + L"defaults.cfg";

        Log::printf(L"Loading default settings \'%s\'...", default_file.c_str());

        if (!loadSettings(default_file, defaultSettings)) {
            Log::print(L"Error: Unable to find default settings file.");
        }

        wstring custom_file = getCustomSettingFilePath();

        Log::printf(L"Loading custom settings \'%s\'...", custom_file.c_str());
        if (!loadSettings(custom_file, customSettings)) {
            Log::print(L"Error: Unable to find custom settings file.");
        }
    }

    Settings::~Settings()
    {
    }

    Settings &Settings::getInstance()
    {
        static Settings defaults;
        return defaults;
    }

    wstring Settings::getCustomSettingFilePath()
    {
        return AppUtils::getBasePath() + L"custom.cfg";
    }

    bool Settings::loadSettings(const wstring &file_name,
                                SettingsMap &settings)
    {
        FILE *file = _wfopen(file_name.c_str(), L"r");
        wifstream settingsStream(file);

        if (!settingsStream) {
            return false;
        }

        const int bufsize = 1024;
        wchar_t buf[bufsize];

        while (settingsStream.getline(buf, bufsize)) {
            wstring line = buf;

            size_t split_at = line.find('=');

            wstring variable(line, 0, split_at);
            variable = AppUtils::removeWhiteSpace(variable);

            wstring value(line, split_at + 1, line.length());
            value = AppUtils::removeWhiteSpace(value);

            settings[variable] = value;
        }

        return true;
    }

    bool Settings::write()
    {
        return writeSettings(getCustomSettingFilePath(),
                             customSettings);
    }

    bool Settings::writeSettings(const wstring &file_name,
                                 SettingsMap &settings)
    {
        FILE *file = _wfopen(file_name.c_str(), L"w");
        wofstream settingsStream(file);

        if (!settingsStream) {
            return false;
        }

        for (SettingsMapIterator i  = settings.begin();
                i != settings.end();
                i++) {
            settingsStream << (*i).first
            << L" = "
            << (*i).second
            << endl;
        }

        return true;
    }


    wstring Settings::getString(const wstring &setting)
    {
        SettingsMapIterator value = customSettings.find(setting);

        if (value != customSettings.end()) {
            return (*value).second;
        }

        value = defaultSettings.find(setting);

        if (value != defaultSettings.end())
            return (*value).second;

        return L"";
    }

    wstring Settings::getPath(const std::wstring &setting)
    {
        return (AppUtils::getBasePath() + getString(setting));
    }

    bool Settings::getBool(const wstring &setting)
    {
        int num = _wtoi(getString(setting).c_str());

        if (num == 0) {
            return false;
        }

        return true;
    }

    int Settings::getInt(const wstring &setting)
    {
        const wstring &value = getString(setting);
        return _wtoi(value.c_str());
    }

    void Settings::set(const std::wstring &setting,
                       const std::wstring &value)
    {
        customSettings[setting] = value;
    }
};