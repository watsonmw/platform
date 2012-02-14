#ifndef VIEWER_DOT_H
#define VIEWER_DOT_H

#include "3dmath.h"
#include "terrain.h"
#include "sky.h"

class Viewer
{
public:
    Viewer();
    virtual ~Viewer();

    // Stores the area of interest
    void lockAOI();

    // Puts the area of interest in the centre of the view
    void lookAtAOI(Vector);

    // Different movement modes
    enum MoveMode {
        MOUSELOOK_MoveMode,
        AREAOFINTEREST_MoveMode,
        MOVETOAREAOFINTEREST_MoveMode,
        POINTLOCK_MoveMode
    };

    bool moveForward;

    // Set viewer move mode
    void setMode(MoveMode newMode);

    void onMouseMove(int x, int y, int dx, int dy, float rx, float ry);

    // Check for colisions with the terrain, translate the world, draw the sky
    void draw(Terrain *terrain);

    // viewer position
    Vector  pos;
    // looking in direction 'rot', give as euler angles(degrees)
    Vector  rot;
    float   speed;

    bool moveBack;
    bool moveLeft;
    bool moveRight;
    bool move3;
    MoveMode mode;

    // position of 'pointer' on screen
    float   sightx;
    float   sighty;
    Vector  straff;

    // Area of interest Paramters
    Vector  aoidir;
    Vector  aoipos;

    // For smoothy rotating the view
    float   interpolate;
    Vector  eyerot;
    Vector  startrot;

    // Area of interest Paramters
    Vector  focuspos;
    Vector  focusdir;

    // Aspect ratio
    GLfloat ar;

private:
    Sky* _sky;

    // Positions the aoi in the right place
    void updateAOI();

    // Moves the viewer 'sideways'
    void applyStraff();

    // Calculates the distance to the area of interest
    float distancetoAOI();

    // Moves the viewer back/forwards based on current rotation
    void applyMovements();

    // Rotate the view to the rotate the value in eyerot
    void moveToEyeRot();
};

#endif
