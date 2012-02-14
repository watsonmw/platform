#ifndef _COMBINEDTEXTURE_DOT_H_INCLUDED_
#define _COMBINEDTEXTURE_DOT_H_INCLUDED_

class Image;

class TextureFragment
{
public:
    double GetU1() {
        return u1;
    }
    double GetV1() {
        return v1;
    }
    double GetU2() {
        return u2;
    }
    double GetV2() {
        return v2;
    }

private:
    double u1, v1;
    double u2, v2;

    friend class CombinedTexture;
};

/**
 * Grabs a set of textures from an image.
 */
class CombinedTexture
{
public:
    CombinedTexture(Image *sourceImage);

    ~CombinedTexture();

    /**
     * Returns the number of texture fragments contained in the image
     */
    int getNumberOfTextureFragments();

    /**
     * Returns a texture fragment
     */
    TextureFragment *getTextureFragment(int index);

    /**
     * Reload the render caches into the current GL context
     */
    void reloadRenderCaches();
};

#endif
