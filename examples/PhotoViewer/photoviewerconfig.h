#ifndef PHOTOVIEWERCONFIG_DOT_H_INCLUDED
#define PHOTOVIEWERCONFIG_DOT_H_INCLUDED

#include <string>
#include <FreeImage.h>
#include "platform/registrysettings.h"

/**
 * Accessor functions for application registry settings
 * Makes it easier to get and set values from the registry
 * Can give default values if the registry is missing a key.
 */
class PhotoViewerConfig
{
public:
    PhotoViewerConfig(Platform::RegistrySettings &registry_settings);

    std::wstring getLastPicturePath();
    void setLastPicturePath(const std::wstring &last_picture_path);
    void resetLastPicturePath() {
        setLastPicturePath(L"");
    }

    std::wstring getBasePicturesPath();
    std::wstring getDefaultBasePicturesPath();
    void setBasePicturesPath(const std::wstring &base_pictures_path);

    int getNextPictureWait();
    int getDefaultNextPictureWait();
    void setNextPictureWait(int milliseconds);

    /**
     * If showSmallImages is set the image loader
     * will use the MinPictureWidth and MinPictureHeight
     * to disgard small images.
     */
    bool showSmallImages();
    void setShowSmallImages(bool show_small_images);
    bool showSmallImagesDefault() {
        return true;
    }

    int getMinPictureWidth();
    int getMinPictureHeight();

    /**
     * Access a list of previously set picture paths.
     * An item is added to this list everytime setBasePicturesPath()
     * is called with a unique path.
     */
    int getNumRememberedPicturePaths();
    std::wstring getRememberedPicturePath(int i);

    /**
     * Set/get the freeimage rescale filter type
     */
    void setRescaleFilterType(FREE_IMAGE_FILTER filterType);
    FREE_IMAGE_FILTER getRescaleFilterType();


private:
    bool isUniquePicturePath(const std::wstring &pic_path);
    void rememberPicturePath(const std::wstring &pic_path);

    std::wstring getMyPicturesPath();
    Platform::RegistrySettings &registry_settings;
};

#endif
