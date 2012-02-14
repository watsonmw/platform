
#include "stdafx.h"

#include "platform/imageloader.h"
#include "platform/log.h"
#include "platform/image.h"
#include "platform/pcximage.h"
#include "platform/tgaimage.h"

#include <string>
#include <cstdio>
#include <algorithm>

using namespace std;

namespace Platform
{
    Image *ImageLoader::LoadImage(const wstring &filePath,
                                  Image::PixelFormat pixelFormat)
    {
        Image *image = NULL;

        // select file type tag
        size_t dotIndex = filePath.find_last_of('.');
        wstring fileType = filePath.substr(dotIndex + 1, filePath.length() - dotIndex);
        transform(fileType.begin(), fileType.end(), fileType.begin(), tolower);

        // select the right image loader.
        if (fileType == L"tga") {
            image = new TGAImage();
            image->setPixelFormat(pixelFormat);
            image->load(filePath);
		}
        else if (fileType == L"pcx") {
            PCXImage pcxImage;
            if (pcxImage.load(filePath)) {
                image = pcxImage.convert(pixelFormat);
            }
		}
        else {
            Log::printf(L"Unknown image file type: \'%s\'", fileType.c_str());
            return NULL;
        }

        return image;
    }
};