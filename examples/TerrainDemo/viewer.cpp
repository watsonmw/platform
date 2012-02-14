#include "stdafx.h"
#include "viewer.h"
#include "quaternion.h"
#include "options.h"

Viewer::Viewer()
{
    zeroVector(pos);
    pos[1] = 26.0;

    zeroVector(rot);
    zeroVector(straff);
    zeroVector(aoidir);
    zeroVector(aoipos);
    zeroVector(eyerot);
    zeroVector(startrot);
    zeroVector(focuspos);
    zeroVector(focusdir);
    
    ar = 1.0;
    speed = 2.0;

    _sky = new Sky();
    _sky->load();

    mode = AREAOFINTEREST_MoveMode;
    interpolate = 1;

    moveForward = false;
    moveBack = false;
    moveLeft = false;
    moveRight = false;
}

void Viewer::setMode(MoveMode newMode)
{
    if (newMode == mode) {
        return;
    }

    mode = newMode;
    if (mode == POINTLOCK_MoveMode) {
        straff[0] = 0.0f;
        straff[1] = 0.0f;
        lockAOI();
    }
}

void Viewer::onMouseMove(int x, int y, int dx, int dy, float rx, float ry)
{
    switch (mode) {
    case MOUSELOOK_MoveMode:
        rot[0] += dy / 6.0f;
        rot[1] += dx / 6.0f;

        if (rot[0] > 85) {
            rot[0] = 85;
        }

        if (rot[0] < -85) {
            rot[0] = -85;
        }

        sightx = 0;
        sighty = 0;
        break;
    case AREAOFINTEREST_MoveMode:
    case MOVETOAREAOFINTEREST_MoveMode:
        sightx = rx;
        sighty = ry;
        straff[0] -= dy / 6.0f;
        straff[1] -= dx / 6.0f;
        break;
    case POINTLOCK_MoveMode:
        straff[0] -= dy / 6.0f;
        straff[1] -= dx / 6.0f;
        break;
    }
}

void Viewer::applyMovements()
{
    Vector forward = { sin((rot[1] / 360)*2*M_PI),
                       -sin((rot[0] / 360)*2*M_PI),
                       -cos((rot[1] / 360)*2*M_PI)
                     };

    Vector forwardMove;
    copyVector(forwardMove, forward);
    scaleVector(forwardMove, speed);

    if (moveForward) {
        addVector(pos, forwardMove, pos);
    }
    else if (moveBack) {
        subtractVector(pos, forwardMove, pos);
    }

    Vector up = { 0.0, 1.0, 0.0 };
    Vector straff;
    crossProduct(up, forward, straff);
    scaleVector(straff, 0.5);

    if (moveLeft) {
        addVector(pos, straff, pos);
    }
    else if (moveRight) {
        subtractVector(pos, straff, pos);
    }
}

void Viewer::moveToEyeRot()
{
    if (interpolate < 1) {
        //FIXME: Use slerp routine instead
        //       or maybe as well as, due to need for eular angles in
        //       calculating aoi
        interpolate += 0.1f;
        rot[0] = startrot[0] * (1 - interpolate) + eyerot[0] * interpolate;
        rot[1] = startrot[1] * (1 - interpolate) + eyerot[1] * interpolate;
        rot[2] = startrot[2] * (1 - interpolate) + eyerot[2] * interpolate;
    }
}

void Viewer::draw(Terrain *terrain)
{
    Vector oldpos;
    Vector newpos;

    copyVector(oldpos, pos);

    switch (mode) {
    case MOUSELOOK_MoveMode:
        applyMovements();
        updateAOI();
        terrain->CalcAreaOfInterest(this);
        break;
    case AREAOFINTEREST_MoveMode:
        updateAOI();
        terrain->CalcAreaOfInterest(this);
        moveToEyeRot();
        applyMovements();
        break;
    case MOVETOAREAOFINTEREST_MoveMode:
        copyVector(startrot, rot);
        lockAOI();
        lookAtAOI(eyerot);
        clampRotationVector(startrot);
        clampRotationVector(eyerot);

        if (startrot[1] - eyerot[1] > 180) {
            eyerot[1] += 360;
        }
        if (startrot[1] - eyerot[1] < -180) {
            eyerot[1] -= 360;
        }

        interpolate = 0;

        updateAOI();
        terrain->CalcAreaOfInterest(this);
        moveToEyeRot();
        applyMovements();
        break;
    case POINTLOCK_MoveMode:
        if (moveForward) {
            pos[0] += speed * focusdir[0];
            pos[1] += speed * focusdir[1];
            pos[2] += speed * focusdir[2];
        }
        else if (moveBack) {
            pos[0] -= speed * focusdir[0];
            pos[1] -= speed * focusdir[1];
            pos[2] -= speed * focusdir[2];
        }
        applyStraff();
        lookAtAOI(rot);
        break;
    }

    if (terrain->Collision(oldpos, pos, newpos)) {
        copyVector(pos, newpos);
    }

    glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rot[2], 0.0f, 0.0f, 1.0f);

    if (opt_sky_draw) {
        if (opt_global_night) {
            glDisable(GL_FOG);
            glColor3f(0.2f, 0.1f, 0.1f);
        }

        _sky->draw();

        if (opt_global_night) {
            glEnable(GL_FOG);
            glColor3f(1, 1, 1);
        }
    }

    glTranslatef(-pos[0], -pos[1], -pos[2]);

    if (opt_terrain_wireframe) {
        glDisable(GL_TEXTURE_2D);
        glColor3f(1, 0, 1);
        glBegin(GL_POINTS);
        glVertex3f(terrain->aoipos[0], terrain->aoipos[1], terrain->aoipos[2]);
        glEnd();
    }
}

void Viewer::applyStraff()
{
    Matrix m;
    Vector mfocuspos, tmp;

    float scale = distancetoAOI();
    if (scale > 200) {
        scale = 200;
    }

    if (scale > 3) {
        float delta =  0.01f * scale * straff[0];

        pos[0] -= delta * focusdir[0];
        pos[1] -= delta * focusdir[1];
        pos[2] -= delta * focusdir[2];
    }

    mfocuspos[0] = -focuspos[0];
    mfocuspos[1] = -focuspos[1];
    mfocuspos[2] = -focuspos[2];

    identityMatrix(m);

    translateMatrix(m, mfocuspos);
    rotateMatrixY(m, degree2rad(straff[1]));
    translateMatrix(m, focuspos);

    multVertexMatrix(pos, m, tmp);

    memcpy(pos, tmp, sizeof(Vector));

    zeroVector(straff);
}

void Viewer::lockAOI()
{
    updateAOI();

    memcpy(focuspos, aoipos, sizeof(Vector));
    memcpy(focusdir, aoidir, sizeof(Vector));
}

float Viewer::distancetoAOI()
{
    float x, y, z;

    x = focuspos[0] - pos[0];
    y = focuspos[1] - pos[1];
    z = focuspos[2] - pos[2];

    return sqrt((x * x) + (y * y) + ( z * z));
}

void Viewer::updateAOI()
{
    Vector zaxis, yaxis, aoirot;
    Matrix aoirotation;
    Matrix deltaxrot, xydeltayrot;
    Quaternion q;

    aoirot[1] = rot[1];
    aoirot[0] = rot[0] + rad2degree(tanh(sighty * tan(degree2rad(15))));
    aoirot[2] = 0.0;

    initVector(zaxis, 0.0, 0.0, -1.0);
    initVector(yaxis, 0.0, -1.0, 0.0);

    identityMatrix(xydeltayrot);

    rotateMatrixX(xydeltayrot, -degree2rad(aoirot[0]));
    rotateMatrixY(xydeltayrot, -degree2rad(aoirot[1]));

    AxisAngleToQuaterion(q, yaxis, tanh(sightx * tan(degree2rad(15*ar))));
    QuaternionToRotationMatrix(q, deltaxrot);

    multMatrix(xydeltayrot, deltaxrot, aoirotation);

    multVectorMatrix(zaxis, aoirotation, aoidir);
}

void Viewer::lookAtAOI(Vector r)
{
    Vector tfocuspos;
    subtractVector(focuspos, pos, tfocuspos);

    memcpy(focusdir, tfocuspos, sizeof(Vector));

    normaliseVector(focusdir);
    lookAtVector(focusdir, r);
}

Viewer::~Viewer()
{
    delete _sky; _sky = 0;
}
