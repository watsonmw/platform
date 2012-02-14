#include "stdafx.h"
#include <math.h>
#include "quaternion.h"

void QuaternionToRotationMatrix(Quaternion q, Matrix m)
{
    m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
    m[1][0] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
    m[2][0] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
    m[3][0] = 0.0f;

    m[0][1] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
    m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
    m[2][1] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
    m[3][1] = 0.0f;

    m[0][2] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
    m[1][2] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
    m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Function:    MultQuaternions
// Purpose:     Computes the product of two quaternions
// Arguments:   pointer to quaternions and dest
///////////////////////////////////////////////////////////////////////////////
void MultQuaternions(Quaternion quat1, Quaternion quat2, Quaternion dest)
{
    Quaternion tmp;
    tmp[0] =        quat2[3] * quat1[0] + quat2[0] * quat1[3] +
                    quat2[1] * quat1[2] - quat2[2] * quat1[1];
    tmp[1] =    quat2[3] * quat1[1] + quat2[1] * quat1[3] +
                quat2[2] * quat1[0] - quat2[0] * quat1[2];
    tmp[2]  =   quat2[3] * quat1[2] + quat2[2] * quat1[3] +
                quat2[0] * quat1[1] - quat2[1] * quat1[0];
    tmp[3]  =   quat2[3] * quat1[3] - quat2[0] * quat1[0] -
                quat2[1] * quat1[1] - quat2[2] * quat1[2];
    dest[0] = tmp[0];
    dest[1] = tmp[1];
    dest[2] = tmp[2];
    dest[3] = tmp[3];
}


///////////////////////////////////////////////////////////////////////////////
// Function:    NormalizeQuaternion
// Purpose:     Normalize a Quaternion
// Arguments:   a quaternion to set
// Discussion:  Quaternions must follow the rules of x^2 + y^2 + z^2 + w^2 = 1
//              This function insures this
///////////////////////////////////////////////////////////////////////////////
void NormalizeQuaternion(Quaternion quat)
{
    float magnitude;

    magnitude = (quat[0] * quat[0]) +
                (quat[1] * quat[1]) +
                (quat[2] * quat[2]) +
                (quat[3] * quat[3]);

    quat[0] = quat[0] / magnitude;
    quat[1] = quat[1] / magnitude;
    quat[2] = quat[2] / magnitude;
    quat[3] = quat[3] / magnitude;
}
// NormalizeQuaternion  ///////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// THESE TWO PROCEDURES ARE FUNCTIONALLY EQUIVALENT.  TWO METHODS TO CONVERT
// A SERIES OF ROTATIONS TO QUATERNIONS.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:    EulerToQuaternion
// Purpose:     Convert a set of Euler angles to a Quaternion
// Arguments:   A rotation set of 3 angles, a quaternion to set
// Discussion:  As the order of rotations is important I am
//              using the Quantum Mechanics convention of (X,Y,Z)
//              a Yaw-Pitch-Roll (Y,X,Z) system would have to be
//              adjusted.  It is more efficient this way though.
///////////////////////////////////////////////////////////////////////////////
void EulerToQuaternion(Vector rot, Quaternion quat)
{
    Quaternion qx, qy, qz, qtmp;
    float rx, ry, rz, tx, ty, tz, cx, cy, cz, sx, sy, sz;

    rx = (rot[0] * (float)M_PI) / (360 / 2);
    ry = (rot[1] * (float)M_PI) / (360 / 2);
    rz = (rot[2] * (float)M_PI) / (360 / 2);

    tx = rx * (float)0.5;
    ty = ry * (float)0.5;
    tz = rz * (float)0.5;
    cx = (float)cos(tx);
    cy = (float)cos(ty);
    cz = (float)cos(tz);
    sx = (float)sin(tx);
    sy = (float)sin(ty);
    sz = (float)sin(tz);

    qx[0] = sx;
    qx[1] = 0;
    qx[2] = 0;
    qx[3] = cx;

    qy[0] = 0;
    qy[1] = sy;
    qy[2] = 0;
    qy[3] = cy;

    qz[0] = 0;
    qz[1] = 0;
    qz[2] = sz;
    qz[3] = cz;

    MultQuaternions( qy, qx, qtmp );
    MultQuaternions( qtmp, qz, quat );

    NormalizeQuaternion(quat);
}

// EulerToQuaternion  /////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:    QuatToAxisAngle
// Purpose:     Convert a Quaternion to Axis Angle representation
// Arguments:   A quaternion to convert, a axisAngle to set
///////////////////////////////////////////////////////////////////////////////
void QuaterionToAxisAngle(Quaternion quat, Axisangle axisAngle)
{
    float scale, tw;

    tw = (float)acos(quat[3]) * 2;
    scale = (float)sin(tw / 2.0);
    axisAngle[1] = quat[0] / scale;
    axisAngle[2] = quat[1] / scale;
    axisAngle[3] = quat[2] / scale;

    axisAngle[0] = (tw * (360 / 2)) / (float)M_PI;
}
// QuatToAxisAngle  /////////////////////////////////////////////////////////

void AxisAngleToQuaterion(Quaternion q, Vector v, float angle)
{
    float sin_a = sin(angle / 2);
    float cos_a = cos(angle / 2);

    q[0] = v[0] * sin_a;
    q[1] = v[1] * sin_a;
    q[2] = v[2] * sin_a;
    q[3] = cos_a;

    NormalizeQuaternion(q);
}

#define DELTA   0.0001      // DIFFERENCE AT WHICH TO LERP INSTEAD OF SLERP

///////////////////////////////////////////////////////////////////////////////
// Function:    SlerpQuat
// Purpose:     Spherical Linear Interpolation Between two Quaternions
// Arguments:   Two Quaternions, blend factor, result quaternion
// Notes:       The comments explain the handling of the special cases.
//              The comments in the magazine were wrong.  Adjust the
//              DELTA constant to play with the LERP vs. SLERP level
///////////////////////////////////////////////////////////////////////////////
void SlerpQuat(Quaternion quat1, Quaternion quat2, float slerp, Quaternion result)
{
    float omega, cosom, sinom, scale0, scale1;

    cosom = quat1[0] * quat2[0] +
            quat1[1] * quat2[1] +
            quat1[2] * quat2[2] +
            quat1[3] * quat2[3];

    if ((1.0 + cosom) > DELTA) {
        if ((1.0 - cosom) > DELTA) {
            omega = acos(cosom);
            sinom = sin(omega);
            scale0 = sin((1.0f - slerp) * omega) / sinom;
            scale1 = sin(slerp * omega) / sinom;
        }
        else {
            scale0 = 1.0f - slerp;
            scale1 = slerp;
        }
        result[0] = scale0 * quat1[0] + scale1 * quat2[0];
        result[1] = scale0 * quat1[1] + scale1 * quat2[1];
        result[2] = scale0 * quat1[2] + scale1 * quat2[2];
        result[3] = scale0 * quat1[3] + scale1 * quat2[3];
    }
    else {
        result[0] = -quat2[1];
        result[1] = quat2[0];
        result[2] = -quat2[3];
        result[3] = quat2[2];
        scale0 = sin((1.0f - slerp) * (float)HALF_PI);
        scale1 = sin(slerp * (float)HALF_PI);
        result[0] = scale0 * quat1[0] + scale1 * result[0];
        result[1] = scale0 * quat1[1] + scale1 * result[1];
        result[2] = scale0 * quat1[2] + scale1 * result[2];
        result[3] = scale0 * quat1[3] + scale1 * result[3];
    }
}
// SlerpQuat  /////////////////////////////////////////////////////////////////
