#ifndef _LOCATION_DOT_H
#define _LOCATION_DOT_H

#include <string>

namespace Platform {
    class PCXImage;
}

class Location
{
public:
    Location();
    virtual ~Location();

    bool load(const std::wstring& mapFile, const std::wstring& placenames);

    char *getPlaceName(int x, int y);

private:
    int getPlaceIdent(int x, int y);

    char *_placename[256];
    char *_text;
    Platform::PCXImage *_map;
    int _places;
};

#endif
