#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "3dmath.h"

#include "platform/log.h"

void clampRotationVector(Vector v)
{
    if (v[0] > 360)
        v[0] -= 360;
    if (v[0] < 0)
        v[0] += 360;

    if (v[1] > 360)
        v[1] -= 360;
    if (v[1] < 0)
        v[1] += 360;

    if (v[2] > 360)
        v[2] -= 360;
    if (v[2] < 0)
        v[2] += 360;
}

// Vector routines
void initVector(Vector v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
void zeroVector(Vector v)
{
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}

void copyVector(Vector d, Vector s)
{
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
}

void normaliseVector(Vector v)
{
    float length;

    length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    length = (float)sqrt(length);

    if (length != 0) {
        v[0] /= length;
        v[1] /= length;
        v[2] /= length;
    }
    else {
        v[0] = v[1] = v[2] = 0;
    }
}

void getPlaneNormal(Vector v1, Vector v2, Vector v3, Vector d)
{
    Vector tmp1, tmp2;

    tmp1[0] = v2[0] - v1[0];
    tmp1[1] = v2[1] - v1[1];
    tmp1[2] = v2[2] - v1[2];
    tmp2[0] = v3[0] - v1[0];
    tmp2[1] = v3[1] - v1[1];
    tmp2[2] = v3[2] - v1[2];

    crossProduct(tmp1, tmp2, d);
    normaliseVector(d);
}

float scalerProduct(Vector v1, Vector v2)
{
    //normaliseVector(v1);normaliseVector(v2);
    return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}

void crossProduct(Vector v1, Vector v2, Vector d)
{
    Vector t;

    t[0] = v1[1] * v2[2] - v1[2] * v2[1];
    t[1] = v1[2] * v2[0] - v1[0] * v2[2];
    t[2] = v1[0] * v2[1] - v1[1] * v2[0];

    d[0] = t[0];
    d[1] = t[1];
    d[2] = t[2];

}

void scaleVector(Vector v, float scale)
{
    v[0] = v[0] * scale;
    v[1] = v[1] * scale;
    v[2] = v[2] * scale;
}

void addVector(Vector v1, Vector v2, Vector d)
{
    d[0] = v1[0] + v2[0];
    d[1] = v1[1] + v2[1];
    d[2] = v1[2] + v2[2];
}

void subtractVector(Vector v1, Vector v2, Vector d)
{
    d[0] = v1[0] - v2[0];
    d[1] = v1[1] - v2[1];
    d[2] = v1[2] - v2[2];
}


void sphericalVector(Vector v, float a, float b)
{
    v[0] = cos(a) * sin(b);
    v[1] = sin(a);
    v[2] = cos(a) * cos(b);
}

void lookAtVector(Vector v, Vector d)
{
    Vector tmp;
    Matrix rot;

    memcpy(tmp, v, sizeof(Vector));
    tmp[1] = 0;
    normaliseVector(tmp);
    d[1] = atan2(tmp[0], -tmp[2]);

    identityMatrix(rot);
    rotateMatrixY(rot, d[1]);
    multVectorMatrix(v, rot, tmp);

    d[0] = -rad2degree(atan2(tmp[1], -tmp[2]));
    d[1] = rad2degree(d[1]);
    d[2] = 0;
}
// Matrix-vertex routines

void multVertexMatrix(Vector v, Matrix m, Vector d)
{
    int i, j;
    float w;

    for (i = 0; i < 3; i++) {
        d[i] = 0;
        for (j = 0; j < 3; j++) {
            d[i] += m[j][i] * v[j];
        }
        d[i] += m[3][i];
    }
    w = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3];

    if (w == 0.0) w = 1.0; //this should never happen....

    d[0] = d[0] / w;
    d[1] = d[1] / w;
    d[2] = d[2] / w;
}

// Matrix-vector routines

void multVectorMatrix(Vector v, Matrix m, Vector d)
{
    int i, j;

    for (i = 0; i < 3; i++) {
        d[i] = 0;
        for (j = 0; j < 3; j++) {
            d[i] += m[j][i] * v[j];
        }
    }
}


// Matrix routines
void translateMatrix(Matrix m, Vector v)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);
    int j;
    for (j = 0; j < 3; j++) {
        tmp[3][j] = v[j];
    }
    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));
}

void identityMatrix(Matrix m)
{
    memset(m, 0, sizeof(Matrix));

    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3][3] = 1;
}

void rotateMatrix(Matrix m, Vector v1, Vector v2, Vector v3)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);

    tmp[0][0] = v1[0];
    tmp[1][0] = v1[1];
    tmp[2][0] = v1[2];
    tmp[0][1] = v2[0];
    tmp[1][1] = v2[1];
    tmp[2][1] = v2[2];
    tmp[0][2] = v3[0];
    tmp[1][2] = v3[1];
    tmp[2][2] = v3[2];

    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));
}

void rotateMatrixX(Matrix m, float angle)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);

    tmp[1][1] = cos(angle);
    tmp[1][2] = sin(angle);
    tmp[2][1] = -sin(angle);
    tmp[2][2] = cos(angle);

    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));

}

void rotateMatrixY(Matrix m, float angle)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);

    tmp[0][0] = cos(angle);
    tmp[0][2] = -sin(angle);
    tmp[2][0] = sin(angle);
    tmp[2][2] = cos(angle);

    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));

}

void rotateMatrixZ(Matrix m, float angle)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);

    tmp[0][0] = cos(angle);
    tmp[0][1] = sin(angle);
    tmp[1][0] = -sin(angle);
    tmp[1][1] = cos(angle);

    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));

}

// ****NOT TESTED****
void lookAtMatrix(Matrix result, Vector v, float roll)
{
    // Get our direction vector (the Z vector component of the matrix)
    // and make sure it's normalized into a unit vector

    Vector zaxis, yaxis, xaxis;
    Matrix m;
    memcpy(zaxis, v, sizeof(Vector));

    normaliseVector(zaxis);

    // Build the Y vector of the matrix (handle the degenerate case
    // in the way that 3DS does) -- This is not the TRUE vector, only
    // a reference vector.

    if (!zaxis[0] && !zaxis[2]) {
        yaxis[0] =  -zaxis[1];
        yaxis[1] =  0.0f;
        yaxis[2] =  0.0f;
    }
    else {
        yaxis[0] =  0.0f;
        yaxis[1] =  1.0f;
        yaxis[2] =  0.0f;
    }

    // Build the X axis vector based on the two existing vectors

    crossProduct(yaxis, zaxis, xaxis);
    normaliseVector(xaxis);

    // Correct the Y reference vector
    crossProduct(xaxis, zaxis, yaxis);
    normaliseVector(yaxis);
    yaxis[0] = -yaxis[0];
    yaxis[1] = -yaxis[1];
    yaxis[2] = -yaxis[2];

    // Generate rotation matrix without roll included
    identityMatrix(m);
    rotateMatrix(m, xaxis, yaxis, zaxis);

    // Generate the Z rotation matrix for roll (bank)
    rotateMatrixZ(m, roll);
}

void perspectiveMatrix(Matrix m, float aspect, float fov, float znear, float zfar)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);

    tmp[0][0] = -cos(fov / 2);
    tmp[1][1] = -cos(fov / 2) * aspect;
    tmp[2][2] = sin(fov / 2) / (1 - (znear / zfar));
    tmp[2][3] = sin(fov / 2);
    tmp[3][2] = -tmp[2][2] * znear;
    tmp[3][3] = 0;

    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));
}

void shadowMatrix(Matrix m, float ypos)
{
    float dot;
    float light[] = { 0.0f, 0.7f, -0.7f, 0.0f};
    float ground[] = { 0.0f, 1.0f, 0.0f, ypos};

    dot = ground[0] * light[0] +
          ground[1] * light[1] +
          ground[2] * light[2] +
          ground[3] * light[3];

    m[0][0] = dot  - light[0] * ground[0];
    m[1][0] = 0.0f - light[0] * ground[1];
    m[2][0] = 0.0f - light[0] * ground[2];
    m[3][0] = 0.0f - light[0] * ground[3];

    m[0][1] = 0.0f - light[1] * ground[0];
    m[1][1] = dot  - light[1] * ground[1];
    m[2][1] = 0.0f - light[1] * ground[2];
    m[3][1] = 0.0f - light[1] * ground[3];

    m[0][2] = 0.0f - light[2] * ground[0];
    m[1][2] = 0.0f - light[2] * ground[1];
    m[2][2] = dot  - light[2] * ground[2];
    m[3][2] = 0.0f - light[2] * ground[3];

    m[0][3] = 0.0f - light[3] * ground[0];
    m[1][3] = 0.0f - light[3] * ground[1];
    m[2][3] = 0.0f - light[3] * ground[2];
    m[3][3] = dot  - light[3] * ground[3];
}

void orthographicMatrix(Matrix m, float scale)
{
    Matrix tmp, tmp2;

    identityMatrix(tmp);

    tmp[0][0] = scale;
    tmp[1][1] = scale;
    tmp[2][2] = scale;
    tmp[3][3] = 1;

    multMatrix(tmp, m, tmp2);
    memcpy(m, tmp2, sizeof(Matrix));
}

void multMatrix(Matrix m1, Matrix m2, Matrix res)
{
    int i, j, k;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            res[j][i] = 0;
            for (k = 0; k < 4; k++) {
                res[j][i] += m1[k][i] * m2[j][k];
            }
        }
    }
}

void printMatrix(Matrix m)
{
    int i;
    Platform::Log::print(L"--------------------------------\n");
    for (i = 0; i < 4; i++) {
        Platform::Log::printf(L"%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
    }
    Platform::Log::print(L"--------------------------------\n");
}

