#include <cmath>
#include <limits>
#include <cstdlib>

#include "render.h"

Matrix translationMatrix(Vec3f v) {
    Matrix Tr = Matrix::identity();
    Tr[0][3] = v.x;
    Tr[1][3] = v.y;
    Tr[2][3] = v.z;
    return Tr;
}

Matrix zoomMatrix(float factor) {
    Matrix Z = Matrix::identity();
    Z[0][0] = Z[1][1] = Z[2][2] = factor;
    return Z;
}

Matrix xRotationMatrix(float cosangle, float sinangle) {
    Matrix R = Matrix::identity();
    R[1][1] = R[2][2] = cosangle;
    R[1][2] = -sinangle;
    R[2][1] =  sinangle;
    return R;
}

Matrix yRotationMatrix(float cosangle, float sinangle) {
    Matrix R = Matrix::identity();
    R[0][0] = R[2][2] = cosangle;
    R[0][2] =  sinangle;
    R[2][0] = -sinangle;
    return R;
}

Matrix zRotationMatrix(float cosangle, float sinangle) {
    Matrix R = Matrix::identity();
    R[0][0] = R[1][1] = cosangle;
    R[0][1] = -sinangle;
    R[1][0] =  sinangle;
    return R;
}
