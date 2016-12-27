// math.c

#include "math.h"

#include <math.h>

float
vector3_magnitude(vector3 input)
{
    return sqrt(input.x * input.x +
		input.y * input.y +
		input.z * input.z);
}

vector3
vector3_subtract(vector3 a, vector3 b)
{
    vector3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

vector3
vector3_normalize(vector3 input)
{
    float magnitude = vector3_magnitude(input);
    float magnitude_inv = 0.0f;
    if(magnitude > 0.0f)
    {
	magnitude_inv = 1.0f / magnitude;
	
    }
    return (vector3){{{input.x * magnitude_inv,
		    input.y * magnitude_inv,
		    input.z * magnitude_inv}}};
}

vector3
vector3_cross(vector3 a, vector3 b)
{
    vector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.y * b.x - a.x * b.y;
    return result;
}

float
vector3_dot(vector3 a, vector3 b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

matrix4
matrix_identity()
{
    matrix4 matrix = {{
	    1.0f, 0.0f, 0.0f, 0.0f,
	    0.0f, 1.0f, 0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f,
	    0.0f, 0.0f, 0.0f, 1.0f
	}};

    return matrix;
}

matrix4
matrix_rotation_x(float rotation)
{
    float theta_sin = sin(rotation);
    float theta_cos = cos(rotation);
    
    matrix4 matrix = {{
	    1.0f, 0.0f, 0.0f, 0.0f,
	    0.0f, theta_cos, -theta_sin, 0.0f,
	    0.0f, theta_sin, theta_cos, 0.0f,
	    0.0f, 0.0f, 0.0f, 1.0f
	}};

    return matrix;
}

matrix4
matrix_rotation_y(float rotation)
{
    float theta_sin = sin(rotation);
    float theta_cos = cos(rotation);
    
    matrix4 matrix = {{
	    theta_cos, 0.0f, theta_sin, 0.0f,
	    0.0f, 1.0f, 0.0f, 0.0f,
	    -theta_sin, 0.0f, theta_cos, 0.0f,
	    0.0f, 0.0f, 0.0f, 1.0f
	}};

    return matrix;
}

matrix4
matrix_rotation_z(float rotation)
{
    float theta_sin = sin(rotation);
    float theta_cos = cos(rotation);
    
    matrix4 matrix = {{
	    theta_cos, -theta_sin, 0.0f, 0.0f,
	    theta_sin, theta_cos, 0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f,
	    0.0f, 0.0f, 0.0f, 1.0f
	}};

    return matrix;
}

matrix4
matrix_translate(float x, float y, float z)
{
    matrix4 matrix = {{
	    1.0f, 0.0f, 0.0f, 0.0f,
	    0.0f, 1.0f, 0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f,
	    x, y, z, 1.0f
	}};

    return matrix;
}

matrix4
matrix_perspective(float field_of_view, float aspect_ratio, float near, float far)
{
    float fov_radians = DEGREES_TO_RADIANS * field_of_view;
    float top = near * tan(fov_radians * 0.5f);
    float bottom = -top;
    float right = top * aspect_ratio;
    float left = -right;
    
    float near2 = near * 2.0f;
    float rml = (right - left);
    float rpl = (right + left);
    float tmb = (top - bottom);
    float tpb = (top + bottom);
    float fmn = (far - near);
    float fpn = (far + near);

    matrix4 matrix = {{
	    near2 / rml, 0.0f, rpl / rml, 0.0f,
	    0.0f, near2 / tmb, tpb / tmb, 0.0f,
	    0.0f, 0.0f, -fpn / fmn, -1.0f,
	    0.0f, 0.0f, -(2.0f * far * near) / fmn, 0.0f
	}};

    return matrix;
}

matrix4
matrix_orthographic(float width, float height, float near, float far)
{
    float fmn = (far - near);
    float fpn = (far + near);

    float r = width * 0.5f;
    float t = height * 0.5f;

    matrix4 matrix = {{
	    1.0f / r, 0.0f, 0.0f, 0.0f,
	    0.0f, 1.0f / t, 0.0f, 0.0f,
	    0.0f, 0.0f, -2.0f / fmn, 0.0f,
	    0.0f, 0.0f, -(fpn / fmn), 1.0f
	}};

    return matrix;
}

matrix4
matrix_look_at(vector3 eye, vector3 at, vector3 up)
{
    vector3 z = vector3_normalize(vector3_subtract(eye, at));
    vector3 x = vector3_normalize(vector3_cross(z, up));
    vector3 y = vector3_cross(x, z);

    float tx = vector3_dot(x, eye);
    float ty = vector3_dot(y, eye);
    float tz = vector3_dot(z, eye);
    
    matrix4 result = (matrix4){{
	    x.x, x.y, x.z, 0.0f,
	    y.x, y.y, y.z, 0.0f,
	    z.x, z.y, z.z, 0.0f,
	    -tx, -ty, -tz, 1.0f
	}};

    return result;
}

matrix4
matrix_look_fps(vector3 eye, float pitch, float yaw)
{
    float pitch_radians = DEGREES_TO_RADIANS * pitch;
    float yaw_radians = DEGREES_TO_RADIANS * yaw;
    
    float cos_pitch = cos(pitch_radians);
    float sin_pitch = sin(pitch_radians);
    float cos_yaw = cos(yaw_radians);
    float sin_yaw = sin(yaw_radians);

    vector3 x = (vector3){{{cos_yaw, 0.0f, sin_yaw}}};
    vector3 y = (vector3){{{sin_yaw * sin_pitch, cos_pitch, -cos_yaw * sin_pitch}}};
    vector3 z = (vector3){{{sin_yaw * cos_pitch, -sin_pitch, -cos_pitch* cos_yaw}}};
    
    float tx = vector3_dot(x, eye);
    float ty = vector3_dot(y, eye);
    float tz = vector3_dot(z, eye);

    matrix4 result = (matrix4){{
	    x.x, x.y, x.z, 0.0f,
	    y.x, y.y, y.z, 0.0f,
	    z.x, z.y, z.z, 0.0f,
	    -tx, -ty, -tz, 1.0f
	}};

    return result;
}
