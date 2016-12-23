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
matrix4 matrix_translate(float x, float y, float z);
matrix4 matrix_perspective(float left, float right, float bottom, float top, float near, float far);
matrix4 matrix_orthographic(float width, float height, float near, float far);

#endif // MATH_H_INCLUDED
