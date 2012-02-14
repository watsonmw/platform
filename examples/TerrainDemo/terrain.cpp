#include "stdafx.h"
#include "terrain.h"
#include "viewer.h"
#include "texture.h"
#include "quaternion.h"
#include "options.h"
#include <math.h>

#include "platform/log.h"
#include "platform/imageloader.h"

using namespace Platform;

#define MORPH_RANGE .025f

////////////////////////////////////////////////////////////////////////////
// Public: Terrain()
// Params:
//      No params.
// Initialises a few variables...
////////////////////////////////////////////////////////////////////////////
Terrain::Terrain()
{
    heightmap = NULL;

    camznear = 0;
    opt_terrain_wireframe = false;

    //set to .1 to see little pops...
    varthreshold = .05f;

    opt_global_debug = false;

    placename = NULL;
    range = 200;

    vertices = new Vector[maxvertices];
}

Terrain::~Terrain()
{
    delete [] texturemap; texturemap = 0;
    delete [] colormap; colormap = 0;
    delete [] varianceTree1; varianceTree1 = 0;
    delete [] varianceTree2; varianceTree2 = 0;
    delete [] bintritree; bintritree = 0;
    delete [] vertices; vertices = 0;
}

////////////////////////////////////////////////////////////////////////////
// Public:  load()
// Params:  charactor array to a tga file repesenting the height array.
// Returns: true if ok, false if failed.
////////////////////////////////////////////////////////////////////////////
bool Terrain::load(const std::wstring& filePath)
{
    // load in a image, brightness repesents the height of the terrain.
    Image* image = ImageLoader::LoadImage(filePath, Image::GRAYSCALE);
    if (!image) {
        Log::print(L"Couldnt load the terrain height map.");
        return false;
    }

    heightmap = image->getWritableData();
    width     = image->getWidth();
    height    = image->getHeight();

    if (height != 256 && width != 256) {
        Log::print(L"Terrain image needs to be 256 * 256 at the moment.(sorry)");
        return false;
    }

    CalcVarianceTree();

    CalcLight();
    CalcTexture();

    // maximum number of bintrees that we could ever need
    // is allocated, so no error checking has to be done.
    bintritree = new BinTriTree[256*256*4]; //(n^2 + n^2) * 2

    location.load(L"data/location.pcx", L"data/location.txt");

    return true;
}

////////////////////////////////////////////////////////////////////////////
// Public: Draw()
// Params: Pointer to Viewer class, uses position and rotations values
////////////////////////////////////////////////////////////////////////////
void Terrain::Draw(Viewer *viewer)
{
    starttime = timeGetTime();

    BuildTree(viewer);

    endtime = timeGetTime();
    Log::printf(L"Split count: %d in %dms", tricount, endtime - starttime);

    glBindTexture(GL_TEXTURE_2D, gltexture);

    tricount = 0;
    starttime = endtime;

    RenderTree();

    endtime = timeGetTime();
    Log::printf(L"Render Count: %d in %dms", tricount, endtime - starttime);
}

////////////////////////////////////////////////////////////////////////////
// Private: RenderTree()
////////////////////////////////////////////////////////////////////////////
void Terrain::RenderTree(void)
{
    if (!opt_terrain_wireframe)
        glBegin(GL_TRIANGLES);

    vertexindex = 0;

    RenderTree2(0, 0, (float)getHeight(0, 0),
                height, width, (float)getHeight(height, width),
                0, width, (float)getHeight(0, width),
                &bintritree[1]);

    RenderTree2(height, width, (float)getHeight(height, width),
                0, 0, (float)getHeight(0, 0),
                height, 0, (float)getHeight(height, 0),
                &bintritree[2]);

    if (!opt_terrain_wireframe) {
        glEnd();
    }
}

////////////////////////////////////////////////////////////////////////////
// Private: RenderTree2()
// Params:  Vertices of current triangle with their morph valuse.
////////////////////////////////////////////////////////////////////////////
void Terrain::RenderTree2(UINT16 x1, UINT16 y1, float morph1,
                          UINT16 x2, UINT16 y2, float morph2,
                          UINT16 x3, UINT16 y3, float morph3,
                          BinTriTree *tri)
{

    if (tri->leftChild != 0 && tri->rightChild != 0) {
        int hx = (x1 + x2) / 2;
        int hy = (y1 + y2) / 2;

        float realHeight = (float)getHeight(hx, hy);

        float avgHeight = (morph1 + morph2) / 2;

        float v = realHeight - avgHeight;

        float tmorph = tri->morph;

        if (tri->bottomNeighbour != 0) {
            tmorph = (tmorph + tri->bottomNeighbour->morph) / 2;
        }

        float morphheight = avgHeight + (v * tmorph);

        RenderTree2(x2, y2, morph2,
                    x3, y3, morph3,
                    hx, hy, morphheight,
                    tri->leftChild);

        RenderTree2(x3, y3, morph3,
                    x1, y1, morph1,
                    hx, hy, morphheight,
                    tri->rightChild);
    }
    else {
        tricount++;

        if (opt_terrain_wireframe) {
#ifdef NICE_WIREFRAME
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.3f, 0.5f, 1.0f);
            glBegin(GL_TRIANGLES);
            glVertex3f((GLfloat)x1 - 128, morph1*0.1f, (GLfloat)y1 - 128);
            glVertex3f((GLfloat)x2 - 128, morph2*0.1f, (GLfloat)y2 - 128);
            glVertex3f((GLfloat)x3 - 128, morph3*0.1f, (GLfloat)y3 - 128);
            glEnd();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 1.0f);
            glEnable(GL_TEXTURE_2D);
            glColor3f(1, 1, 1);

            glBegin(GL_TRIANGLES);
            glTexCoord2f(x1 / 256.0f, y1 / 256.0f);
            glVertex3f((GLfloat)x1 - 128, morph1*0.1f, (GLfloat)y1 - 128);

            glTexCoord2f(x2 / 256.0f, y2 / 256.0f);
            glVertex3f((GLfloat)x2 - 128, morph2*0.1f, (GLfloat)y2 - 128);

            glTexCoord2f(x3 / 256.0f, y3 / 256.0f);
            glVertex3f((GLfloat)x3 - 128, morph3*0.1f, (GLfloat)y3 - 128);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glEnd();
#else
            // BASIC WIREFRAME
            glDisable(GL_TEXTURE_2D); // shouldn't be here really
            glBegin(GL_LINE_LOOP);
            glVertex3f((GLfloat)x1 - 128, morph1*0.1f, (GLfloat)y1 - 128);
            glVertex3f((GLfloat)x2 - 128, morph2*0.1f, (GLfloat)y2 - 128);
            glVertex3f((GLfloat)x3 - 128, morph3*0.1f, (GLfloat)y3 - 128);
            glEnd();
#endif
        }
        else {
            glTexCoord2f(x1 / 256.0f, y1 / 256.0f);
            glVertex3f((GLfloat)x1 - 128, morph1*0.1f, (GLfloat)y1 - 128);

            glTexCoord2f(x2 / 256.0f, y2 / 256.0f);
            glVertex3f((GLfloat)x2 - 128, morph2*0.1f, (GLfloat)y2 - 128);

            glTexCoord2f(x3 / 256.0f, y3 / 256.0f);
            glVertex3f((GLfloat)x3 - 128, morph3*0.1f, (GLfloat)y3 - 128);
        }
    }
}

////////////////////////////////////////////////////////////////////////////
// Private: BuildTree()
// Params:  Pointer to the viewer class.
////////////////////////////////////////////////////////////////////////////
void Terrain::BuildTree(Viewer *viewer)
{
    bintritree[1].leftChild = 0;
    bintritree[1].rightChild = 0;
    bintritree[1].leftNeighbour = 0;
    bintritree[1].rightNeighbour = 0;
    bintritree[1].bottomNeighbour = &bintritree[2];
    bintritree[1].leaf = 1;

    bintritree[2].leftChild = 0;
    bintritree[2].rightChild = 0;
    bintritree[2].leftNeighbour = 0;
    bintritree[2].rightNeighbour = 0;
    bintritree[2].bottomNeighbour = &bintritree[1];
    bintritree[2].leaf = 1;

    nextfreebintree = 3;

    Vector d1, d2, d3, d4;

    DistanceTo(d1, -128, -128, viewer->pos, viewer->rot);
    DistanceTo(d2, -128, 128, viewer->pos, viewer->rot);
    DistanceTo(d3,  128, 128, viewer->pos, viewer->rot);
    DistanceTo(d4,  128, -128, viewer->pos, viewer->rot);
    DistanceTo(aoipos, -viewer->aoipos[2], -viewer->aoipos[0],
               viewer->pos, viewer->rot);

    tricount = 0;
    if (!aoi) {
        aoipos[0] = 20000;
        aoipos[2] = 20000;
    }

    cvarianceTree = varianceTree1;
    ctreedepth = 17;
    //  BuildTree2(&bintritree[1], d1[2], d3[2], d2[2]);
    BuildTree3(&bintritree[1], d1[0], d1[2], d3[0], d3[2], d2[0], d2[2]);

    cvarianceTree = varianceTree2;
    ctreedepth = 17;
    //  BuildTree2(&bintritree[2], d3[2], d1[2], d4[2]);
    BuildTree3(&bintritree[2], d3[0], d3[2], d1[0], d1[2], d4[0], d4[2]);
}

////////////////////////////////////////////////////////////////////////////
// Private: DistanceTo()
// Params:  Vector:       a vector to hold the result
//          float, float: a 2d point on the terrain.
//          Vector:       the view position.
//          Vector:       the view direction.(Given in degrees)
//
// Side effects: Vector result is changed.
////////////////////////////////////////////////////////////////////////////
void Terrain::DistanceTo(Vector result, float x, float y,
                         Vector view, Vector viewrot)
{
    Vector rot;
    Matrix matrix;

    rot[0] = -view[0];
    rot[1] = 0;
    rot[2] = -view[2];

    zeroVector(result);

    identityMatrix(matrix);
    translateMatrix(matrix, rot);
    rotateMatrixY(matrix, ((viewrot[1] - 180) / 360)*2*M_PI);

    rot[0] = -y;
    rot[2] = -x;
    multVertexMatrix(rot, matrix, result);
}

////////////////////////////////////////////////////////////////////////////
// Private: BuildTree3()
// Params:  BinTriTree:   actually a node, but hey.
//          float, float: right vertex
//          float, float: left vertex
//          float, float: apex vertex
// Side effects: BinTriTree is built.
//               ctreedepth changes by only relevant to recursion.
////////////////////////////////////////////////////////////////////////////
void Terrain::BuildTree3(BinTriTree *tri, float x1, float z1,
                         float x2, float z2, float x3, float z3)
{
    // get our split point
    float zh = (z1 + z2) / 2;
    float xh = (x1 + x2) / 2;
    float variance;

    // Are we the lowest triangle in tree
    if (ctreedepth < 1)
        return;

    int cullstate = TriCull2D(x1, z1, x2, z2, x3, z3);

    // Are we in view fustrum?
    if (cullstate == 2)
        return;

    variance = ((float)cvarianceTree[tri->leaf]) / abs(zh);

    // AOI calculation
    float d = ((aoipos[0] - xh) * (aoipos[0] - xh) + (aoipos[2] - zh) * (aoipos[2] - zh));

    if (d < range) {
        // Scale the variance
        variance = variance * (1 + ((range - d) * invrange));
    }

    if ( variance > varthreshold) {
        // Split triangle
        float t = variance - varthreshold;
        if (t < MORPH_RANGE) {
            // Geomorphing
            tri->morph = t / MORPH_RANGE;
        }
        else {
            tri->morph = 1.0f;
        }

        if (tri->leftChild == 0 && tri->rightChild == 0) {
            // Has not been split, so split it
            Split(tri);
        }

        ctreedepth--;
        if (cullstate == 1) {
            // Triangle totally inside view fustrum
            BuildTree2(tri->rightChild, x2, z2, x3, z3, xh, zh);
            BuildTree2(tri->leftChild, x3, z3, x1, z1, xh, zh);
        }
        else {
            // Needs further checking
            BuildTree3(tri->rightChild, x2, z2, x3, z3, xh, zh);
            BuildTree3(tri->leftChild, x3, z3, x1, z1, xh, zh);
        }
        ctreedepth++;
    }
}

////////////////////////////////////////////////////////////////////////////
// Private: BuildTree2()
// Params:  BinTriTree:   actually a node, but hey.
//          float, float: right vertex
//          float, float: left vertex
//          float, float: apex vertex
// Side effects: BinTriTree is built.
//               ctreedepth changes by only relevant to recursion.
////////////////////////////////////////////////////////////////////////////
void Terrain::BuildTree2(BinTriTree *tri, float x1, float z1,
                         float x2, float z2, float x3, float z3)
{
    // get our split point
    float zh = (z1 + z2) / 2;
    float xh = (x1 + x2) / 2;
    float variance;

    // Are we the lowest triangle in tree
    if (ctreedepth < 1)
        return;

    variance = ((float)cvarianceTree[tri->leaf]) / abs(zh);

    // AOI calculation
    float d = ((aoipos[0] - xh) * (aoipos[0] - xh) + (aoipos[2] - zh) * (aoipos[2] - zh));

    if (d < range) {
        // Scale the variance
        variance = variance * (1 + ((range - d) * invrange));
    }

    if ( variance > varthreshold) {
        // Split triangle
        float t = variance - varthreshold;
        if (t < MORPH_RANGE) {
            // Geomorphing
            tri->morph = t / MORPH_RANGE;
        }
        else {
            tri->morph = 1.0;
        }

        if (tri->leftChild == 0 && tri->rightChild == 0) {
            // Has not been split, so split it
            Split(tri);
        }

        ctreedepth--;
        BuildTree2(tri->rightChild, x2, z2, x3, z3, xh, zh);
        BuildTree2(tri->leftChild, x3, z3, x1, z1, xh, zh);
        ctreedepth++;
    }
}

////////////////////////////////////////////////////////////////////////////
// Private: Split()
// Params:  BinTriTree: triangle to be split.
//
// Side effects: BinTriTree is split the mesh kept coherent.
////////////////////////////////////////////////////////////////////////////
void Terrain::Split(BinTriTree *tri)
{
    if (tri->bottomNeighbour != 0) {
        if (tri->bottomNeighbour->bottomNeighbour != tri) {
            Split(tri->bottomNeighbour);
        }
        Split2(tri);
        Split2(tri->bottomNeighbour);
        tri->leftChild->rightNeighbour = tri->bottomNeighbour->rightChild;
        tri->rightChild->leftNeighbour = tri->bottomNeighbour->leftChild;
        tri->bottomNeighbour->leftChild->rightNeighbour = tri->rightChild;
        tri->bottomNeighbour->rightChild->leftNeighbour = tri->leftChild;
    }
    else {
        Split2(tri);
        tri->leftChild->rightNeighbour = 0;
        tri->rightChild->leftNeighbour = 0;
    }
}

////////////////////////////////////////////////////////////////////////////
// Private: Split2()
// Params:  BinTriTree: triangle to be split.
//
// Side effects: BinTriTree is split.
////////////////////////////////////////////////////////////////////////////
void Terrain::Split2(BinTriTree *tri)
{
    tri->leftChild = &bintritree[nextfreebintree++];
    tri->rightChild = &bintritree[nextfreebintree++];

    tri->leftChild->leaf = tri->leaf * 2;
    tri->rightChild->leaf = (tri->leaf * 2) + 1;

    tri->leftChild->morph = 0;
    tri->rightChild->morph = 0;

    tri->leftChild->leftNeighbour = tri->rightChild;
    tri->rightChild->rightNeighbour = tri->leftChild;
    tri->leftChild->bottomNeighbour = tri->leftNeighbour;

    if (tri->leftNeighbour != 0) {
        if (tri->leftNeighbour->bottomNeighbour == tri) {
            tri->leftNeighbour->bottomNeighbour = tri->leftChild;
        }
        else {
            if (tri->leftNeighbour->leftNeighbour == tri) {
                tri->leftNeighbour->leftNeighbour = tri->leftChild;
            }
            else {
                tri->leftNeighbour->rightNeighbour = tri->leftChild;
            }
        }
    }

    tri->rightChild->bottomNeighbour = tri->rightNeighbour;

    if (tri->rightNeighbour != 0) {
        if (tri->rightNeighbour->bottomNeighbour == tri) {
            tri->rightNeighbour->bottomNeighbour = tri->rightChild;
        }
        else {
            if (tri->rightNeighbour->rightNeighbour == tri) {
                tri->rightNeighbour->rightNeighbour = tri->rightChild;
            }
            else {
                tri->rightNeighbour->leftNeighbour = tri->rightChild;
            }
        }
    }

    tri->leftChild->leftChild = 0;
    tri->leftChild->rightChild = 0;
    tri->rightChild->leftChild = 0;
    tri->rightChild->rightChild = 0;

    tricount++;
}

////////////////////////////////////////////////////////////////////////////
// Private: insideTri()
// Params:  float, float: left vertex of triangle.
//          float, float: right vertex of triangle.
//          float, float: apex of triangle.
//          float, float: point tob checked.
// Return:  Is point inside triangle.
// Side effects: None.
////////////////////////////////////////////////////////////////////////////
inline bool Terrain::insideTri(float x1, float y1, float x2, float y2,
                               float x3, float y3, float a, float b)
{
    //FIXME: faster(?) if you get orientation of 1,2,ab and 1,3,ab
    float d;
    float c;

    c = ((x2 - x1) * y1) - ((y2 - y1) * x1);
    d = (((x2 - x1) * b) - ((y2 - y1) * a)) - c;

    if (d > 0)
        return false;

    c = ((x3 - x2) * y2) - ((y3 - y2) * x2);
    d = (((x3 - x2) * b) - ((y3 - y2) * a)) - c;

    if (d > 0)
        return false;

    c = ((x1 - x3) * y3) - ((y1 - y3) * x3);
    d = (((x1 - x3) * b) - ((y1 - y3) * a)) - c;

    if (d > 0)
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////
// Private: CalcVarianceTree()
//
// Side effects: Built variance tree. varianceTree1 and varianceTree2
////////////////////////////////////////////////////////////////////////////
void Terrain::CalcVarianceTree()
{
    int v;

    varianceTree1 = new UINT8[256*256*2];
    varianceTree2 = new UINT8[256*256*2];

    // a level more than this makes no sence... i think...
    maxtreedepth = 15;
    ctreedepth = 0;

    cvarianceTree = varianceTree1;
    v = CalcVarianceTreeTM(0, 0, height, width, 0, width, 1);
    Log::printf(L"max variance: %d.", v);

    cvarianceTree = varianceTree2;
    v = CalcVarianceTreeTM(height, width, 0, 0,  height, 0, 1);
    Log::printf(L"max variance: %d.", v);
}

////////////////////////////////////////////////////////////////////////////
// Private: CalcVarianceTreeTM()
// Params: UINT16, UINT16: right vertex
//         UINT16, UINT16: left vertex
//         UINT16, UINT16: apex vertex
//         int:            leaf number in variance tree.
// Return: variance of leaf.
////////////////////////////////////////////////////////////////////////////
int Terrain::CalcVarianceTreeTM(UINT16 x1, UINT16 y1, UINT16 x2,
                                UINT16 y2, UINT16 x3, UINT16 y3, int leaf)
{

    int hx = (x1 + x2) / 2;
    int hy = (y1 + y2) / 2;

    int realHeight = getHeight(hx, hy);

    int avgHeight = (getHeight(x1, y1) + getHeight(x2, y2)) / 2;

    int v = abs(realHeight - avgHeight);

    //stop recursion when grid size is less than 1
    if (ctreedepth < 14) {
        ctreedepth++;
        int v1 = CalcVarianceTreeTM(x2, y2, x3, y3, hx, hy, leaf * 2);
        if (v1 > v) v = v1;

        ctreedepth++;
        v1 = CalcVarianceTreeTM(x3, y3, x1, y1, hx, hy, (leaf * 2) + 1);
        if (v1 > v) v = v1;
    }
    else if (ctreedepth < maxtreedepth) {
        ctreedepth++;
        int v1 = CalcVarianceTree((float)x2, (float)y2,
                                  (float)x3, (float)y3,
                                  (float)hx, (float)hy,
                                  leaf * 2);
        if (v1 > v) v = v1;

        ctreedepth++;
        v1 = CalcVarianceTree((float)x3, (float)y3,
                              (float)x1, (float)y1,
                              (float)hx, (float)hy,
                              (leaf * 2) + 1);
        if (v1 > v) v = v1;
    }

    ctreedepth--;
    cvarianceTree[leaf] = (UINT8)v;
    return v;
}

////////////////////////////////////////////////////////////////////////////
// Private:  CalcVarianceTree()
// Params:   float, float: right vertex
//           float, float: left vertex
//           float, float: apex vertex
//           int:          leaf number in variance tree.
// Return:   variance of leaf.
// Comments: used for small triagles - greater accuracy
////////////////////////////////////////////////////////////////////////////
int Terrain::CalcVarianceTree(float x1, float y1, float x2,
                              float y2, float x3, float y3, int leaf)
{

    Vector v1;
    Vector v2;
    Vector v3;
    Vector normal;

    float hx = (x1 + x2) / 2;
    float hy = (y1 + y2) / 2;

    float realHeight = (float)getHeight((UINT16)hx, (UINT16)hy);

    float avgHeight = (getHeight((UINT16)x1, (UINT16)y1) +
                       getHeight((UINT16)x2, (UINT16)y2)) / 2.0f;

    float v = abs(realHeight - avgHeight);

    v1[0] = x1;
    v1[1] = getHeight((UINT16)x1, (UINT16)y1) * 0.1f;
    v1[2] = y1;

    v2[0] = x2;
    v2[1] = getHeight((UINT16)x2, (UINT16)y2) * 0.1f;
    v2[2] = y2;

    v3[0] = x3;
    v3[1] = getHeight((UINT16)x3, (UINT16)y3) * 0.1f;
    v3[2] = y3;

    getPlaneNormal(v1, v2, v3, normal);

    normal[1] = abs(normal[1]);

    normal[1] += (1 - normal[1]) / 2;

    v = v * abs(normal[1]);

    //stop recursion when grid size is less than 1;
    if (ctreedepth < maxtreedepth) {
        ctreedepth++;
        float v1 = (float)
                   CalcVarianceTree((float)x2, (float)y2,
                                    (float)x3, (float)y3,
                                    (float)hx, (float)hy,
                                    leaf * 2);
        if (v1 > v) v = v1;

        ctreedepth++;
        v1 = (float)
             CalcVarianceTree((float)x3, (float)y3,
                              (float)x1, (float)y1,
                              (float)hx, (float)hy,
                              (leaf * 2) + 1);
        if (v1 > v) v = v1;
    }

    ctreedepth--;
    cvarianceTree[leaf] = (UINT8)v;
    return (int)v;
}

////////////////////////////////////////////////////////////////////////////
// Private:  getHeight()
// Params:   UINT16, UINT16: location on terrain
// Return:   height value
// Comments: wraps.
////////////////////////////////////////////////////////////////////////////
inline int Terrain::getHeight(UINT16 x, UINT16 y)
{
    x = x & 255;
    y = y & 255;
    return (int)heightmap[x+(y*256)];
}

////////////////////////////////////////////////////////////////////////////
// Private:  getColor()
// Params:   UINT16, UINT16: location on terrain
// Return:   colour value
// Comments: wraps.
////////////////////////////////////////////////////////////////////////////
inline int Terrain::getColor(UINT16 x, UINT16 y)
{
    x = x & 255;
    y = y & 255;
    return (int)colormap[x+(y*256)];
}

////////////////////////////////////////////////////////////////////////////
// Private:  TriCull2D()
// Params:   float, float: right vertex of triangle
//           float, float: left vertex of triangle
//           float, float: apex vertex of triangle
// Comments: the triangle is previously rotated, fov is 90, and in 2d
// Return:   1 if fully inside.
//           2 if fully outside.
//           0 if partly inside.
////////////////////////////////////////////////////////////////////////////
inline int Terrain::TriCull2D(float x1, float y1, float x2, float y2, float x3, float y3)
{
    int a = PointCull2D(x1, y1);
    int b = PointCull2D(x2, y2);
    int c = PointCull2D(x3, y3);

    //if fully inside
    if (a == 0 && b == 0 && c == 0)
        return 1;

    // if partly inside
    if (a == 0 || b == 0 || c == 0)
        return 0;

    if (LineCull2D(a, b, x1, y1, x2, y2))
        return 0;

    if (LineCull2D(b, c, x2, y2, x3, y3))
        return 0;

    if (LineCull2D(c, a, x3, y3, x1, y1))
        return 0;

    // else tri outside view
    return 2;
}

////////////////////////////////////////////////////////////////////////////
// Private: LineCull2D();
// Params:  ini, int:     values from point cull, related to the other params
//          float, float: start of line
//          float, float: end of line
// Return:  1 if inside or maybe inside(!)
//          0 if outside.
////////////////////////////////////////////////////////////////////////////
inline int Terrain::LineCull2D(int a, int b, float x1, float y1, float x2, float y2)
{
    // behind
    if ((a&b) == 4)
        return 0;

    // infront and oppisite
    if ((a | b) == 3)
        return 1;

    if ((a + b) == 7)
        return 1; // need more checks...
    else
        return 0;
}

////////////////////////////////////////////////////////////////////////////
// Private: PointCull2D()
//
// Is point in view fustrum?
////////////////////////////////////////////////////////////////////////////
inline int Terrain::PointCull2D(float x, float y)
{
    // if behind camera
    if (y < camznear) {
        // if to the left
        if ((y - camznear) < x) {
            return 5;
        }
        // if to the right
        if ((y - camznear) < -x) {
            return 6;
        }
        return 4;
    }
    else {
        // if to the left
        if ((y - camznear) < x) {
            return 1;
        }
        // if to the right
        if ((y - camznear) < -x) {
            return 2;
        }
        // else point inside so..
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////
// Private:  Collision()
//
// Comments: 'Slide' collision is done.
////////////////////////////////////////////////////////////////////////////
int Terrain::Collision(Vector oldpos, Vector pos, Vector newpos)
{
    Vector diff, normal, v1, v2, v3;

    int x = (int)oldpos[0] + 128;
    int y = (int)oldpos[2] + 128;

    newpos[0] = pos[0];
    newpos[1] = pos[1];
    newpos[2] = pos[2];

    if ((x < 0) || (x > 254) || (y < 0) || (y > 254))
        return 0;

    if ( (pos[0] - (x - 128)) < (pos[2] - (y - 128)) ) {
        v1[0] = x - 128.0f;
        v1[1] = getHeight(x, y) * 0.1f;
        v1[2] = y - 128.0f;
        v2[0] = x - 127.0f;
        v2[1] = getHeight(x + 1, y + 1) * 0.1f;
        v2[2] = y - 127.0f;
        v3[0] = x - 127.0f;
        v3[1] = getHeight(x + 1, y) * 0.1f;
        v3[2] = y - 128.0f;
    }
    else {
        v1[0] = x - 128.0f;
        v1[1] = getHeight(x, y) * 0.1f;
        v1[2] = y - 128.0f;
        v2[0] = x - 128.0f;
        v2[1] = getHeight(x, y + 1) * 0.1f;
        v2[2] = y - 127.0f;
        v3[0] = x - 127.0f;
        v3[1] = getHeight(x + 1, y + 1) * 0.1f;
        v3[2] = y - 127.0f;
    }
    getPlaneNormal(v1, v2, v3, normal);

    subtractVector(pos, v1, diff);

    float d = scalerProduct(normal, diff);

    if (d < 1.2f) {
        newpos[0] -= normal[0] * (d - 1.2f);
        newpos[1] -= normal[1] * (d - 1.2f);
        newpos[2] -= normal[2] * (d - 1.2f);
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////
// Private:  CalcLight()
//
// Comments: Cacluates a lightmap for the texture.
//           The light is at infinity.
////////////////////////////////////////////////////////////////////////////
void Terrain::CalcLight()
{
    float d, d1, d2;
    Vector light, normal, v1, v2, v3;

    light[0] = 0.7f;
    light[1] = 0.5f;
    light[2] = -0.5f;

    colormap = new UINT8[256*256];

    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x += 1) {
            v1[0] = x - 128.0f;
            v1[1] = getHeight(x, y) * 0.1f;
            v1[2] = y - 128.0f;
            v2[0] = x - 127.0f;
            v2[1] = getHeight(x + 1, y + 1) * 0.1f;
            v2[2] = y - 127.0f;
            v3[0] = x - 127.0f;
            v3[1] = getHeight(x + 1, y) * 0.1f;
            v3[2] = y - 128.0f;

            getPlaneNormal(v1, v2, v3, normal);

            d1 = scalerProduct(normal, light);

            v1[0] = x - 128.0f;
            v1[1] = getHeight(x, y) * 0.1f;
            v1[2] = y - 128.0f;
            v2[0] = x - 128.0f;
            v2[1] = getHeight(x, y + 1) * 0.1f;
            v2[2] = y - 127.0f;
            v3[0] = x - 127.0f;
            v3[1] = getHeight(x + 1, y + 1) * 0.1f;
            v3[2] = y - 127.0f;

            getPlaneNormal(v1, v2, v3, normal);

            d2 = scalerProduct(normal, light);

            d = (d1 + d2) / 2;

            if (d > 0.2f) {
                colormap[(y*256)+x] = (UINT8)(d * 255.0f);
            }
            else {
                colormap[(y*256)+x] = (UINT8)(0.2f * 255.0f);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////
// Private:  CalcTexture()
//
// Comments: Generates a texture for the terrain.
////////////////////////////////////////////////////////////////////////////
void Terrain::CalcTexture()
{
    texturemap = new UINT8[256*256*3];

    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            if (getHeight(x, y) == 0) {
                texturemap[((y*256)+x)*3] = 0;
                texturemap[(((y*256)+x)*3)+1] = 100;
                texturemap[(((y*256)+x)*3)+2] = 180;
            }
            else {
                texturemap[((y*256)+x)*3] = 32;
                texturemap[(((y*256)+x)*3)+1] = getColor(x, y);
                texturemap[(((y*256)+x)*3)+2] = getHeight(x, y);
            }
        }
    }

    glGenTextures(1, &gltexture);
    glBindTexture(GL_TEXTURE_2D, gltexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texturemap);
}

////////////////////////////////////////////////////////////////////////////
// Private:  CalcAreaOfInterest()
//
// Comments: Calculates where the viewer is looking on the terrain.
////////////////////////////////////////////////////////////////////////////
void Terrain::CalcAreaOfInterest(Viewer *viewer)
{
    aoipos[0] = viewer->pos[0] + 128;
    aoipos[1] = viewer->pos[1];
    aoipos[2] = viewer->pos[2] + 128;

    aoi = false;
    placename = "Void";

    if (aoipos[1] > 25.6f) {
        if (viewer->aoidir[1] >= 0.0f) return;

        float s = (aoipos[1] - 25.6f) / viewer->aoidir[1];
        aoipos[0] -= s * viewer->aoidir[0];
        aoipos[1] -= s * viewer->aoidir[1];
        aoipos[2] -= s * viewer->aoidir[2];
    }
    else if (aoipos[1] < 0.0f) {
        if (viewer->aoidir[1] < 0.0f) return;

        float s = (aoipos[1]) / viewer->aoidir[1];
        aoipos[0] -= s * viewer->aoidir[0];
        aoipos[1] -= s * viewer->aoidir[1];
        aoipos[2] -= s * viewer->aoidir[2];
    }

    bool bDone = false;
    while (!bDone) {
        if (aoipos[0] < 0 && viewer->aoidir[0] < 0) {
            return;
        }

        if (aoipos[0] > 256 && viewer->aoidir[0] > 0) {
            return;
        }

        if (aoipos[2] < 0 && viewer->aoidir[2] < 0) {
            return;
        }

        if (aoipos[2] > 256 && viewer->aoidir[2] > 0) {
            return;
        }

        if (aoipos[1] < (getHeight((UINT16)aoipos[0], (UINT16)aoipos[2]) / 10)) {
            bDone = true;
            break;
        }

        aoipos[0] += viewer->aoidir[0];
        aoipos[1] += viewer->aoidir[1];
        aoipos[2] += viewer->aoidir[2];
    }

    placename = location.getPlaceName((int)aoipos[0],
                                      255 - (int)aoipos[2]);

    aoipos[0] += -128;
    aoipos[1] += 0;
    aoipos[2] += -128;

    viewer->aoipos[0] = aoipos[0];
    viewer->aoipos[1] = aoipos[1];
    viewer->aoipos[2] = aoipos[2];

    range = sqrt(((aoipos[0] - viewer->pos[0]) * (aoipos[0] - viewer->pos[0])) +
                 ((aoipos[1] - viewer->pos[1]) * (aoipos[1] - viewer->pos[1])) +
                 ((aoipos[2] - viewer->pos[2]) * (aoipos[2] - viewer->pos[2])));

    range = range * 10;
    invrange =  3 / range;

    aoi = true;
}

