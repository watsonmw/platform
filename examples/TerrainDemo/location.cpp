#include "stdafx.h"

#include <stdio.h>
#include "platform/log.h"
#include "platform/pcximage.h"

#include "location.h"
#include "pcx.h"

using namespace Platform;

Location::Location()
  : _map(NULL),
    _text(NULL),
    _places(0)
{
}

Location::~Location()
{
    delete _map; _map = NULL;
    delete _text; _text = NULL;
}

bool Location::load(const std::wstring& mapFile, const std::wstring& placenames)
{
    _map = new PCXImage();
    if (!_map->load(mapFile)) {
        Log::printf(L"Unable to load location mapping file.");
        return false;
    }

    FILE *fp = _wfopen(placenames.c_str(), L"r");
    if (fp == NULL) {
        Log::printf(L"Unable to open %s for reading.", placenames);
        return false;
    }

    _text = new char[256*256];
    int size = fread(_text, 1, 256 * 256, fp);

    _places = 0;

    int j = 0;
    Log::printf(L"filesize: %d", size);
    for (int i = 0; i < size; i++) {
        if (_text[i] == '\n') {
            _text[i] = 0;
            _placename[_places] = _text + j;
            Log::printf(L"%d %s", _places, _placename[_places]);
            j = i + 1;
            _places++;
        }
    }
    Log::printf(L"filesize: %d", _places);
    fclose(fp);

    return true;
}

int Location::getPlaceIdent(int x, int y)
{
    if (x < 0 || x > _map->getWidth() || y < 0 || y > _map->getHeight()) {
        return _places;
    }

    return _map->getPaletteIndex(x, y);
}

char *Location::getPlaceName(int x, int y)
{
    int p = getPlaceIdent(x, y);
    if (p < 0 || p >= _places) {
        return "Unknown Grounds";
    }

    return _placename[p];
}
