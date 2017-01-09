#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#define MATH_PI 3.14159265359f
#define MATH_PIOVER2 1.57079632679f

#define DEGREES_TO_RADIANS (MATH_PI / 180.0f)

typedef struct
{
    union {
	struct {
	    float x;
	    float y;
	} position;
	float data[2];
    };
} vector2;

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

float vector3_magnitude(vector3 input);
vector3 vector3_add(vector3 a, vector3 b);
vector3 vector3_subtract(vector3 a, vector3 b);
vector3 vector3_normalize(vector3 input);
vector3 vector3_cross(vector3 a, vector3 b);
float vector3_dot(vector3 a, vector3 b);

vector3 vector3_matrix_multiply(matrix4 matrix, vector3 input);

matrix4 matrix_identity();
matrix4 matrix_rotation_x(float rotation);
matrix4 matrix_rotation_y(float rotation);
matrix4 matrix_rotation_z(float rotation);
matrix4 matrix_rotation_pitch_yaw(float pitch, float yaw);
matrix4 matrix_translate(float x, float y, float z);
matrix4 matrix_multiply(matrix4 a, matrix4 b);
matrix4 matrix_perspective(float field_of_view, float aspect_ratio, float near, float far);
matrix4 matrix_orthographic(float width, float height, float near, float far);
matrix4 matrix_look_at(vector3 eye, vector3 at, vector3 up);
matrix4 matrix_look_fps(vector3 eye, float pitch, float yaw);

#endif // MATH_H_INCLUDED
