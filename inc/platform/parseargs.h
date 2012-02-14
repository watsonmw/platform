#ifndef _PLATFORM_PARSEARGS_H_INCLUDED
#define _PLATFORM_PARSEARGS_H_INCLUDED

#include <vector>
#include <string>

namespace Platform
{
    class ParseArgs
    {
    public:
        ParseArgs(const std::wstring &line);
        ParseArgs(LPTSTR args);
        virtual ~ParseArgs();

        size_t getNumberOfArgs();
        std::wstring getArg(int arg);

        size_t getNumberOfOptions();
        std::wstring getOption(int i);
        std::wstring getOptionParam(int i);

    protected:
        void commonConstructor(const std::wstring &line);
        void parseOptions();
        bool isOption(const std::wstring &arg);

        std::vector<std::wstring> args;

        struct Option {
            std::wstring name;
            std::wstring param;
        };

        std::vector<Option> options;
    };
};
#endif