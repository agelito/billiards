// opengl_glx.c

#include "opengl.h"

#include <GL/glx.h>

void* gl_get_address(const GLubyte* function)
{
    return (void*)glXGetProcAddress(function);
}
