#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#define MATH_PI 3.14159265359f
#define MATH_PIOVER2 1.57079632679f

typedef struct
{
    union {
	struct {
	    float x;
	    float y;
	    float z;
	};
	float data[3];
    };
} vector3;

typedef struct
{
    float data[16];
} matrix4;

matrix4 matrix_identity();
matrix4 matrix_rotation_x(float rotation);
matrix4 matrix_rotation_y(float rotation);
matrix4 matrix_rotation_z(float rotation);
matrix4 matrix_translate(float x, float y, float z);
matrix4 matrix_perspective(float field_of_view, float aspect_ratio, float near, float far);
matrix4 matrix_orthographic(float width, float height, float near, float far);
matrix4 matrix_camera_view();

#endif // MATH_H_INCLUDED
