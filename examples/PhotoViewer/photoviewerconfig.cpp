#include "stdafx.h"

#include "photoviewerconfig.h"
#include "platform/filepath.h"
#include "platform/filelist.h"
#include "platform/win32.h"
#include "platform/registrysettings.h"

#include <shlobj.h>

using namespace std;
using namespace Platform;

PhotoViewerConfig::PhotoViewerConfig (RegistrySettings &registry_settings)
    : registry_settings(registry_settings)
{
    // Remember the default value so it always comes up
    // in the remembered path list
    rememberPicturePath(getBasePicturesPath());
}

wstring PhotoViewerConfig::getLastPicturePath()
{
    return registry_settings.getString(L"last_picture");
}

void PhotoViewerConfig::setLastPicturePath(const wstring &last_picture_path)
{
    registry_settings.set(L"last_picture", last_picture_path);
}

wstring PhotoViewerConfig::getBasePicturesPath()
{
    wstring base_picture_path = registry_settings.getString(L"pictures_folder");

    if (!base_picture_path.empty()) {
        return base_picture_path;
    }

    return getDefaultBasePicturesPath();
}

void PhotoViewerConfig::setBasePicturesPath(const wstring &base_pictures_path)
{
    wstring cleanedup_path = FilePath::cleanUpPath(base_pictures_path);
    registry_settings.set(L"pictures_folder", cleanedup_path);
    if (isUniquePicturePath(cleanedup_path)) {
        rememberPicturePath(cleanedup_path);
    }
}

int PhotoViewerConfig::getNextPictureWait()
{
    int milliseconds = registry_settings.getInt(L"next_picture_wait");
    if (milliseconds > 0) {
        return milliseconds;
    }

    return getDefaultNextPictureWait();
}

void PhotoViewerConfig::setNextPictureWait(int milliseconds)
{
    registry_settings.set(L"next_picture_wait", milliseconds);
}

wstring PhotoViewerConfig::getDefaultBasePicturesPath()
{
    return Win32::getMyPicturesPath();
}

int PhotoViewerConfig::getDefaultNextPictureWait()
{
    return 6000;
}

int PhotoViewerConfig::getMinPictureWidth()
{
    int width = registry_settings.getInt(L"min_picture_width");
    if (width > 0) {
        return width;
    }

    return 300;
}

int PhotoViewerConfig::getMinPictureHeight()
{
    int height = registry_settings.getInt(L"min_picture_height");
    if (height > 0) {
        return height;
    }

    return 300;
}

bool PhotoViewerConfig::showSmallImages()
{
    return registry_settings.getBool(L"show_small_images",
                                     showSmallImagesDefault());
}

void PhotoViewerConfig::setShowSmallImages(bool show_small_images)
{
    registry_settings.set(L"show_small_images", show_small_images);
}

int PhotoViewerConfig::getNumRememberedPicturePaths()
{
    return registry_settings.count_list(L"pic_paths");
}

std::wstring PhotoViewerConfig::getRememberedPicturePath(int i)
{
    return registry_settings.get_list_value(L"pic_paths", i);
}

bool PhotoViewerConfig::isUniquePicturePath(const std::wstring &pic_path)
{
    for (int i = 0; i < registry_settings.count_list(L"pic_paths"); i++) {
        if (pic_path == registry_settings.get_list_value(L"pic_paths", i)) {
            return true;
        }
    }
    return false;
}

void PhotoViewerConfig::rememberPicturePath(const std::wstring &pic_path)
{
    registry_settings.add_list_value(L"pic_paths", pic_path);
}

void PhotoViewerConfig::setRescaleFilterType(FREE_IMAGE_FILTER filter_type)
{
    registry_settings.set(L"rescale_filter_type", filter_type);
}

FREE_IMAGE_FILTER PhotoViewerConfig::getRescaleFilterType()
{
    if (registry_settings.exists(L"rescale_filter_type")) {
        int filter_type = registry_settings.getInt(L"rescale_filter_type");
        return (FREE_IMAGE_FILTER)filter_type;
    }

    return FILTER_CATMULLROM;
}