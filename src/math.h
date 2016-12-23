#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

typedef struct
{
    float data[16];
} matrix4;

matrix4 matrix_identity();
matrix4 matrix_rotation_x(float rotation);
matrix4 matrix_rotation_y(float rotation);
matrix4 matrix_rotation_z(float rotation);

#endif // MATH_H_INCLUDED
