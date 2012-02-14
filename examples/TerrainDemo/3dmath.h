#ifndef MATRIXMATH_DOT_H
#define MATRIXMATH_DOT_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

#define HALF_PI M_PI/2

#define degree2rad(angle) \
    (angle*M_PI/180)

#define rad2degree(angle) \
    (angle*180/M_PI)

// a general vector structure(also used for points)
// different routines are used for vertices
// and vectors
typedef float Vector[3];      // X,Y,Z,1

typedef float Matrix[4][4];
// [ROW][COLUMN]
// [0][0]  [0][1]    [0][2]  [0][3] [0][0]  [0][1]  [0][2]  [0][3]
//                                  [1][0]  [1][1]  [1][2]  [1][3]
//                                  [2][0]  [2][1]  [2][2]  [2][3]
//                                  [3][0]  [3][1]  [3][2]  [3][3]

void clampRotationVector(Vector v);
void zeroVector(Vector v);
void initVector(Vector v, float x, float y, float z);
void copyVector(Vector d, Vector s);

void normaliseVector(Vector v);
void addVector(Vector v1, Vector v2, Vector d);
void subtractVector(Vector v1, Vector v2, Vector d);
void scaleVector(Vector v, float x);
void crossProduct(Vector v1, Vector v2, Vector d);
float scalerProduct(Vector v1, Vector v2);

void lookAtVector(Vector v, Vector d);
void sphericalVector(Vector v, float x, float y);

void getPlaneNormal(Vector v1, Vector v2, Vector v3, Vector d);

void multVertexMatrix(Vector v, Matrix m, Vector res);
void multVectorMatrix(Vector v, Matrix m, Vector res);
void multMatrix(Matrix m1, Matrix m2, Matrix res);


void identityMatrix(Matrix m);

void rotateMatrix(Matrix m, Vector v1, Vector v2, Vector v3);
void rotateMatrixX(Matrix m, float angle);
void rotateMatrixY(Matrix m, float angle);
void rotateMatrixZ(Matrix m, float angle);
void translateMatrix(Matrix m, Vector v);

void lookAtMatrix(Matrix result, Vector v, float roll);
void perspectiveMatrix(Matrix m, float aspect, float fov, float znear, float zfar);
void orthographicMatrix(Matrix m, float scale);
void shadowMatrix(Matrix m, float ypos);

void printMatrix(Matrix m);

#endif
