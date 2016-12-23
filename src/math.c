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

