#ifndef OPENGLWINDOW_DOT_H
#define OPENGLWINDOW_DOT_H

#include "3dmath.h"
/// Quaternion Definitions ////////////////////////////////////////////////////
typedef float Quaternion[4];
typedef float Axisangle[4];
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:    EulerToQuaternion
// Purpose:     Convert a set of Euler angles to a Quaternion
// Arguments:   A rotation set of 3 angles, a quaternion to set
///////////////////////////////////////////////////////////////////////////////
void EulerToQuaternion(Vector , Quaternion );

void QuaternionToRotationMatrix(Quaternion, Matrix);
void MultQuaternions(Quaternion quat1, Quaternion quat2, Quaternion dest);
void AxisAngleToQuaterion(Quaternion q, Vector v, float angle);
///////////////////////////////////////////////////////////////////////////////
// Function:    QuatToAxisAngle
// Purpose:     Convert a Quaternion to Axis Angle representation
// Arguments:   A quaternion to convert, a axisAngle to set
// Discussion:  As the order of rotations is important I am
//              using the Quantum Mechanics convention of (X,Y,Z)
//              a Yaw-Pitch-Roll (Y,X,Z) system would have to be
//              adjusted
///////////////////////////////////////////////////////////////////////////////
void QuaterionToAxisAngle(Quaternion quat, Axisangle axisAngle);

///////////////////////////////////////////////////////////////////////////////
// Function:    SlerpQuat
// Purpose:     Spherical Linear Interpolation Between two Quaternions
// Arguments:   Two Quaternions, blend factor, result quaternion
///////////////////////////////////////////////////////////////////////////////
void SlerpQuat(Quaternion , Quaternion , float slerp, Quaternion);

#endif
