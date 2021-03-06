#ifndef OPENGL_H_INCLUDED
#define OPENGL_H_INCLUDED

#include <GL/gl.h>

#define GL_PROC_DEC(function, ...) typedef void (APIENTRYP GL_PROC_##function) (__VA_ARGS__)
#define GL_PROC_DEC_RET(function, return_type, ...) typedef return_type (APIENTRYP GL_PROC_##function) (__VA_ARGS__)
#define GL_PROC_DEF(function) GL_PROC_##function function
#define GL_PROC_ADDR(function) (GL_PROC_##function)gl_get_address((const GLubyte*)#function)

GL_PROC_DEC(glGenVertexArrays, GLsizei n, GLuint* arrays);
GL_PROC_DEC(glBindVertexArray, GLuint array);
GL_PROC_DEC(glGenBuffers, GLsizei n, GLuint* buffers);
GL_PROC_DEC(glBindBuffer, GLenum target, GLuint buffer);
GL_PROC_DEC(glBufferData, GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
GL_PROC_DEC(glBufferSubData, GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
GL_PROC_DEC(glEnableVertexAttribArray, GLuint index);
GL_PROC_DEC(glVertexAttribPointer, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
GL_PROC_DEC_RET(glCreateShader, GLuint, GLenum );
GL_PROC_DEC(glShaderSource, GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
GL_PROC_DEC(glCompileShader, GLuint shader);
GL_PROC_DEC(glGetShaderiv, GLuint shader, GLenum pname, GLint* params);
GL_PROC_DEC(glGetShaderInfoLog, GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
GL_PROC_DEC_RET(glCreateProgram, GLuint, void);
GL_PROC_DEC(glAttachShader, GLuint program, GLuint shader);
GL_PROC_DEC(glLinkProgram, GLuint program);
GL_PROC_DEC(glGetProgramiv, GLuint program, GLenum pname, GLint* params);
GL_PROC_DEC(glGetProgramInfoLog, GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
GL_PROC_DEC(glUseProgram, GLuint program);
GL_PROC_DEC_RET(glGetUniformLocation, GLint, GLuint program, const GLchar* name);
GL_PROC_DEC(glGetActiveUniform, GLuint program, GLuint index, GLsizei buffSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
GL_PROC_DEC_RET(glGetAttribLocation, GLint, GLuint program, const GLchar* name);

GL_PROC_DEC(glUniformMatrix2fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_PROC_DEC(glUniformMatrix3fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_PROC_DEC(glUniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_PROC_DEC(glUniform1fv, GLint location, GLsizei count, const GLfloat* value);
GL_PROC_DEC(glUniform2fv, GLint location, GLsizei count, const GLfloat* value);
GL_PROC_DEC(glUniform3fv, GLint location, GLsizei count, const GLfloat* value);
GL_PROC_DEC(glUniform4fv, GLint location, GLsizei count, const GLfloat* value);

GL_PROC_DEC(glUniform1iv, GLint location, GLsizei count, const GLint* value);
GL_PROC_DEC(glUniform2iv, GLint location, GLsizei count, const GLint* value);
GL_PROC_DEC(glUniform3iv, GLint location, GLsizei count, const GLint* value);
GL_PROC_DEC(glUniform4iv, GLint location, GLsizei count, const GLint* value);

GL_PROC_DEC(glGenerateMipmap, GLenum target);

typedef struct
{
    GL_PROC_DEF(glGenVertexArrays);
    GL_PROC_DEF(glBindVertexArray);
    GL_PROC_DEF(glGenBuffers);
    GL_PROC_DEF(glBindBuffer);
    GL_PROC_DEF(glBufferData);
    GL_PROC_DEF(glBufferSubData);
    GL_PROC_DEF(glEnableVertexAttribArray);
    GL_PROC_DEF(glVertexAttribPointer);
    GL_PROC_DEF(glCreateShader);
    GL_PROC_DEF(glShaderSource);
    GL_PROC_DEF(glCompileShader);
    GL_PROC_DEF(glGetShaderiv);
    GL_PROC_DEF(glGetShaderInfoLog);
    GL_PROC_DEF(glCreateProgram);
    GL_PROC_DEF(glAttachShader);
    GL_PROC_DEF(glLinkProgram);
    GL_PROC_DEF(glGetProgramiv);
    GL_PROC_DEF(glGetProgramInfoLog);
    GL_PROC_DEF(glUseProgram);
    GL_PROC_DEF(glGetUniformLocation);
    GL_PROC_DEF(glGetActiveUniform);
    GL_PROC_DEF(glGetAttribLocation);

    GL_PROC_DEF(glUniformMatrix2fv);
    GL_PROC_DEF(glUniformMatrix3fv);
    GL_PROC_DEF(glUniformMatrix4fv);
    
    GL_PROC_DEF(glUniform1fv);
    GL_PROC_DEF(glUniform2fv);
    GL_PROC_DEF(glUniform3fv);
    GL_PROC_DEF(glUniform4fv);

    GL_PROC_DEF(glUniform1iv);
    GL_PROC_DEF(glUniform2iv);
    GL_PROC_DEF(glUniform3iv);
    GL_PROC_DEF(glUniform4iv);    
    
    GL_PROC_DEF(glGenerateMipmap);
} gl_functions;

void* gl_get_address(const GLubyte* function);

gl_functions load_gl_functions();

#endif // OPENGL_H_INCLUDED
