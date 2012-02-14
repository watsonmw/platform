#ifndef TERRAIN_DOT_H
#define TERRAIN_DOT_H

#include "3dmath.h"
#include "location.h"
#include <string>

class Viewer;

typedef struct BinTriTree {
    BinTriTree *leftChild;
    BinTriTree *rightChild;

    BinTriTree *leftNeighbour;
    BinTriTree *rightNeighbour;
    BinTriTree *bottomNeighbour;

    // Leaf number in hypothetical fully populated tree.
    int     leaf;

    // how much of real height to render. between 0 and 1.
    float   morph;
    int     vindex;
} BinTriTree;

class Terrain
{
public:
    // The poistion of the are of interest
    Vector aoipos;
    // Current place name that is the area of interest
    char *placename;

    Terrain();
    virtual ~Terrain();

    bool load(const std::wstring& filePath);

    void Draw(Viewer *viewer);
    int Collision(Vector oldpos, Vector pos, Vector newpos);

    // Update the area of interest using given the position and direction
    // of the viewer
    void CalcAreaOfInterest(Viewer *viewer);

private:
    // Calculate the lightmap fro the terrain.
    void CalcLight();

    // Using the lightmap calculate the texture.
    void CalcTexture();

    // Calculate the varance tree for the terrain.
    void CalcVarianceTree();

    // Recursive function to calculate the variance
    // used for very small trangles
    int CalcVarianceTree(float x1, float y1, float x2,
                         float y2, float x3, float y3, int leaf);

    // Recursive function to calculate the variance
    // uses a simpler method
    int CalcVarianceTreeTM(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2,
                           UINT16 x3, UINT16 y3, int leaf);

    // Recursive bintree split function
    void Split(BinTriTree *tri);

    // Basic bintree split function
    // Creates new trangles and links them together with their
    // neighbours.
    void Split2(BinTriTree *tri);

    // Build the bintree structure given the position and direction of the viewer
    void BuildTree(Viewer *viewer);

    // Recursive function to build the tree. This one doesn't checks for clipping.
    void BuildTree2(BinTriTree *tri, float y1, float z1,
                    float y2, float z2, float y3, float z3);

    // Recursive function to build the tree. This one checks for clipping. If triangle
    // is totally inside it calls the BuildTree2 instead of itself.
    void BuildTree3(BinTriTree *tri, float y1, float z1,
                    float y2, float z2, float y3, float z3);

    // Renders the current bintree structure.
    void RenderTree();
    // Recursive funtion to render the tree.
    void RenderTree2(UINT16 x1, UINT16 y1, float morph1,
                     UINT16 x2, UINT16 y2, float morph2,
                     UINT16 x3, UINT16 y3, float morph3,
                     BinTriTree *tri);

    // Function to get the height of the terrain at location x,y.
    // wraps arround.
    int getHeight(UINT16 x, UINT16 y);

    // Function to get the color of the terrain at location x,y.
    // wraps arround.
    int getColor(UINT16 x, UINT16 y);

    // Checks  to see if a point is inside a triangles. used for 2d clipping.
    bool insideTri(float x1, float y1, float x2, float y2, float x3, float y3,
                   float a, float b);

    // Rotates and translates x,y by view and viewrot and stores the result
    // in result(supprise).
    void DistanceTo(Vector result, float x, float y,
                    Vector view, Vector viewrot);

    // Check to see if a tranfromed triangle is inside the view fustrum.
    int TriCull2D(float x1, float y1, float x2, float y2, float x3, float y3);
    // Check to see if a transformed point is inside the view fustrum.
    int PointCull2D(float x, float y);
    // Check to see if a transformed line is inside the view fustrum.
    int LineCull2D(int a, int b, float x1, float y1, float x2, float y2);

    // Data for texture mapping
    UINT8 *heightmap;
    UINT8 *colormap;
    UINT8 *texturemap;
    GLuint gltexture;

    // Height and width of terrain
    int height;
    int width;

    // Two variance trees are needed to repesent a square of terrain.
    // Variance trees are implicit binary trees of per-computed values.
    // repesening how much a triangles children deviate from their parent.
    UINT8 *varianceTree1;
    UINT8 *varianceTree2;

    // The current variance tree being needed.
    UINT8 *cvarianceTree;

    // Current threshold, when the computed variance for a triangle goes
    // above this itys split. To get the pixel tolerance from this value
    // devide by 10, and multiply by half the screen height.
    float varthreshold;

    // A pool of bintrees, allocated to be the max size possible
    // when the terrain is load, in reality it only uses a small
    // fraction of this. Doesn't matter because of the virtual memory
    // allocation works.
    BinTriTree *bintritree;

    // index of next free BinTriTree. set to 0 every frame.
    int nextfreebintree;

    // current tree depth
    int ctreedepth;
    // Maximum depth to render terrain at.
    int maxtreedepth;

    // Current number of trianlges rendered
    int tricount;

    // Start and end times
    DWORD starttime, endtime;

    // Clipping distance
    float camznear;

    // The direction the area of interest is in.
    Vector  aoirot;
    // Is the area of intest on the terrain?
    bool    aoi;
    // The distance to the area of interest
    float   range;
    // 1/range, used quite a lot, precalulated for extra speed
    float   invrange;

    // The location class acciated with this terrain.
    Location location;

    Vector *vertices;
    int vertexindex;
    static const int maxvertices = 10000;
};

#endif
