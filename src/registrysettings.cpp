#include "stdafx.h"

#include "platform/registrysettings.h"
#include "platform/apputils.h"
#include "platform/log.h"

#include <string>
#include <cstdlib>

using namespace std;

namespace Platform
{
    RegistrySettings::RegistrySettings(const wstring &application_name)
            : application_name(application_name)
    {
    }

    RegistrySettings::~RegistrySettings()
    {
    }

    wstring RegistrySettings::getString(const wstring &setting)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return L"";
		}

        DWORD data_type;
        const int BUFFER_SIZE = 1024;
        TCHAR data[BUFFER_SIZE];
        DWORD data_size = BUFFER_SIZE;
        LONG error_code = RegQueryValueEx(application_key,
                                          setting.c_str(),
                                          0,
                                          &data_type,
                                          (BYTE*)data,
                                          &data_size);

        RegCloseKey(application_key);

        if (error_code != ERROR_SUCCESS ||
                (data_type != REG_SZ && data_type != REG_EXPAND_SZ)) {
            return L"";
		}

        return wstring(data);
    }

    LONG RegistrySettings::openApplicationKey(HKEY &application_key)
    {
        wstring key_name = L"SOFTWARE\\WatsonWare\\" +
                           application_name;

        return RegCreateKeyEx(HKEY_CURRENT_USER,
                              key_name.c_str(),
                              0,
                              NULL,
                              REG_OPTION_NON_VOLATILE,
                              KEY_READ | KEY_WRITE,
                              NULL,
                              &application_key,
                              NULL
                             );
    }


    bool RegistrySettings::getBool(const wstring &setting, bool defaultValue)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return 0;
		}

        DWORD data_type;
        DWORD data;
        DWORD data_size = sizeof(DWORD);
        LONG error_code = RegQueryValueEx(application_key,
                                          setting.c_str(),
                                          0,
                                          &data_type,
                                          (BYTE*) & data,
                                          &data_size);

        RegCloseKey(application_key);

        if (error_code != ERROR_SUCCESS ||
                data_type != REG_DWORD)
            return defaultValue;

        if (data == 0) {
            return false;
		}
        else {
            return true;
		}
    }

    int RegistrySettings::getInt(const wstring &setting)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return 0;
		}

        DWORD data_type;
        DWORD data;
        DWORD data_size = sizeof(DWORD);
        LONG error_code = RegQueryValueEx(application_key,
                                          setting.c_str(),
                                          0,
                                          &data_type,
                                          (BYTE*) & data,
                                          &data_size);

        RegCloseKey(application_key);

        if (error_code != ERROR_SUCCESS ||
                (data_type != REG_DWORD)) {
            return 0;
		}

        return data;
    }


    void RegistrySettings::set(const std::wstring &setting,
                               const std::wstring &value)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return;
        }

        BYTE *value_data = (BYTE *)value.c_str();
        RegSetValueEx(application_key,
                      setting.c_str(),
                      0,
                      REG_SZ,
                      value_data,
                      (DWORD)(value.length() * sizeof(wchar_t)));

        RegCloseKey(application_key);
    }


    void RegistrySettings::set(const std::wstring &setting,
                               int value)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return;
        }

        RegSetValueEx(application_key,
                      setting.c_str(),
                      0,
                      REG_DWORD,
                      (BYTE*)&value,
                      sizeof(DWORD));

        RegCloseKey(application_key);
    }


    void RegistrySettings::set(const std::wstring &setting,
                               bool value)
    {
        if (value) {
            set(setting, 1);
        }
        else {
            set(setting, 0);
        }
    }

    int RegistrySettings::count_list(const std::wstring &list)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return 0;
        }

        HKEY list_key;
        if (RegCreateKeyEx(application_key,
                           list.c_str(),
                           0,
                           0,
                           REG_OPTION_NON_VOLATILE,
                           KEY_READ | KEY_WRITE,
                           NULL,
                           &list_key,
                           NULL) != ERROR_SUCCESS) {
            RegCloseKey(application_key);
            return 0;
        }

        DWORD num_sub_keys = 0;
        RegQueryInfoKey(
            list_key, NULL, NULL, NULL, NULL,
            NULL, NULL, &num_sub_keys, NULL, NULL, NULL, NULL);

        RegCloseKey(application_key);
        RegCloseKey(list_key);
        return (int)num_sub_keys;
    }

    std::wstring RegistrySettings::get_list_value(const std::wstring &list, int i)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS)
            return L"";

        HKEY list_key;
        if (RegCreateKeyEx(application_key,
                           list.c_str(),
                           0,
                           0,
                           REG_OPTION_NON_VOLATILE,
                           KEY_READ | KEY_WRITE,
                           NULL,
                           &list_key,
                           NULL) != ERROR_SUCCESS) {
            RegCloseKey(application_key);
            return L"";
        }

        std::wstring value_string;

        const int MAX_VALUE_NAME = 256;
        TCHAR valueName[MAX_VALUE_NAME];
        DWORD valueNameSize = MAX_VALUE_NAME * sizeof(wchar_t);
        DWORD valueType;
        TCHAR valueData[MAX_PATH+1];
        DWORD valueSize = MAX_PATH * sizeof(wchar_t);

        if (RegEnumValue(list_key,
                         i,
                         valueName,
                         &valueNameSize,
                         NULL,
                         &valueType,
                         (LPBYTE)valueData,
                         &valueSize) == ERROR_SUCCESS) {
            if (valueType == REG_SZ) {
                valueData[valueSize / sizeof(wchar_t)] = 0;
                value_string = valueData;
            }
        }

        RegCloseKey(application_key);
        RegCloseKey(list_key);
        return value_string;
    }

    void RegistrySettings::add_list_value(const std::wstring &list,
                                          const std::wstring &value)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return;
		}

        HKEY list_key;
        if (RegCreateKeyEx(application_key,
                           list.c_str(),
                           0,
                           0,
                           REG_OPTION_NON_VOLATILE,
                           KEY_READ | KEY_WRITE,
                           NULL,
                           &list_key,
                           NULL) != ERROR_SUCCESS) {
            RegCloseKey(application_key);
            return;
        }

        // Check if value already exists
        DWORD num_sub_keys = 0;
        RegQueryInfoKey(
            list_key, NULL, NULL, NULL, NULL,
            NULL, NULL, &num_sub_keys, NULL, NULL, NULL, NULL);

        int new_value_id = 0;
        for (size_t i = 0; i < num_sub_keys; i++) {
            const int MAX_VALUE_NAME = 256;
            TCHAR valueName[MAX_VALUE_NAME];
            DWORD valueNameSize = MAX_VALUE_NAME * sizeof(wchar_t);
            DWORD valueType;
            TCHAR valueData[MAX_PATH+1];
            DWORD valueSize = MAX_PATH * sizeof(wchar_t);

            if (RegEnumValue(list_key,
                             i,
                             valueName,
                             &valueNameSize,
                             NULL,
                             &valueType,
                             (LPBYTE)valueData,
                             &valueSize) == ERROR_SUCCESS) {
                if (valueType == REG_SZ) {
                    valueData[valueSize / sizeof(wchar_t)] = 0;
                    wstring value_string = valueData;

                    if (value_string == value)
                        return;

                    TCHAR buffer[20];
                    _itow(new_value_id, buffer, 10 );
                    wstring new_value_id_string = buffer;

                    valueName[valueNameSize] = 0;
                    wstring value_name_string = valueName;

                    if (new_value_id_string == value_name_string)
                        new_value_id++;
                }
            }
        }

        TCHAR buffer[20];
        _itow(new_value_id, buffer, 10);
        BYTE *value_data = (BYTE *)value.c_str();
        RegSetValueEx(list_key,
                      buffer,
                      0,
                      REG_SZ,
                      value_data,
                      (DWORD)(value.length() * sizeof(wchar_t)));

        RegCloseKey(application_key);
        RegCloseKey(list_key);
    }

    bool RegistrySettings::exists(const std::wstring &setting)
    {
        HKEY application_key;
        if (openApplicationKey(application_key) != ERROR_SUCCESS) {
            return false;
		}

        HKEY key;
        bool keyExists = false;
        if (RegOpenKey(application_key, setting.c_str(), &key) == ERROR_SUCCESS) {
            RegCloseKey(key);
            keyExists = true;
        }

        RegCloseKey(application_key);
        return keyExists;
    }
};


