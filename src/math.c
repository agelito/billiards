// math.c

#include "math.h"

#include <math.h>

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
matrix_perspective(float left, float right, float bottom, float top, float near, float far)
{
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

