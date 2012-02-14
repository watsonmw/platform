#include "StdAfx.h"
#include "platform/parseargs.h"

using namespace std;

namespace Platform
{
    ParseArgs::ParseArgs(LPTSTR line)
    {
        commonConstructor(wstring(line));
    }

    ParseArgs::ParseArgs(const wstring &line)
    {
        commonConstructor(line);
    }

    void ParseArgs::commonConstructor(const wstring &line)
    {
        enum ParsingState {
            PARSING_ARG,
            LOOKING_FOR_ARG,
            FINISHED
        };

        ParsingState state = LOOKING_FOR_ARG;
        wstring arg;
        int i = 0;
        while (state != FINISHED) {
            wchar_t charactor = line[i];

            switch (charactor) {
            case ' ':
            case '\0':
                if (state == PARSING_ARG) {
                    state = LOOKING_FOR_ARG;
                    args.push_back(arg);
                    arg.clear();
                }
                break;
            default:
                arg += line[i];
                state = PARSING_ARG;
                break;
            }

            i++;

            if (charactor == 0) {
                break;
            }
        }

        parseOptions();
    }

    ParseArgs::~ParseArgs()
    {
    }

    void ParseArgs::parseOptions()
    {
        for (size_t i = 0; i < getNumberOfArgs(); i++) {
            std::wstring arg = args[i];
            if (isOption(arg)) {
                Option option;
                size_t end = arg.find(':', 1);
                if (end == std::wstring::npos) {
                    option.name = std::wstring(arg, 1, arg.length() - 1);
                }
                else {
                    option.name = std::wstring(arg, 1, end - 1);
                    option.param = std::wstring(arg,
                                                end + 1,
                                                arg.length() - end - 1);
                }
                options.push_back(option);
            }
        }
    }

    size_t ParseArgs::getNumberOfArgs()
    {
        return args.size();
    }

    std::wstring ParseArgs::getArg(int arg)
    {
        return args[arg];
    }

    size_t ParseArgs::getNumberOfOptions()
    {
        return options.size();
    }

    std::wstring ParseArgs::getOption(int index)
    {
        return options[index].name;
    }

    std::wstring ParseArgs::getOptionParam(int index)
    {
        return options[index].param;
    }

    bool ParseArgs::isOption(const std::wstring &arg)
    {
        if (arg.size() < 2)
            return false;

        wchar_t firstCharactor = arg.at(0);

        if (firstCharactor == '/' ||
                firstCharactor == '-')
            return true;

        return false;
    }
};