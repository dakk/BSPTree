
#include <GL/glew.h>

#include "Shader.h"

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

#define GL_DEBUG_MESSAGES print_GL_Error(__FILE__, __LINE__)


static int print_GL_Error(const char *file, int line)
{
        GLenum glErr;
        int retCode = 0;
        glErr = glGetError();
        while (glErr != GL_NO_ERROR)
        {
                printf ("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
                retCode = 1;
                glErr = glGetError();
        }
        return retCode;
}


Shader::Shader () : shader_program(0),
                    vertex_shader(0),
                    fragment_shader(0),
                    vertex_shader_size(0),
                    fragment_shader_size(0)
{}


Shader::~Shader ()
{
        if (has_vertex_shader())   glDeleteShader(vertex_shader);
        if (has_fragment_shader()) glDeleteShader(fragment_shader_size);
        glDeleteProgram(shader_program);
}


void Shader::loadFromFile(const string & vertex_shader_filename, const string & fragment_shader_filename)
{
        const GLchar *vertex_shader_source;
        const GLchar *fragment_shader_source;

        if (vertex_shader_filename   != "") vertex_shader_source = read_shader_source(vertex_shader_filename, vertex_shader_size);
        if (fragment_shader_filename != "") fragment_shader_source = read_shader_source(fragment_shader_filename, fragment_shader_size);
        shader_program = glCreateProgram();

        if (has_vertex_shader ())
        {
                compile_attach(vertex_shader, GL_VERTEX_SHADER, &vertex_shader_source);
                delete [] vertex_shader_source;
        }

        if (has_fragment_shader ())
        {
                compile_attach(fragment_shader, GL_FRAGMENT_SHADER, &fragment_shader_source);
                delete [] fragment_shader_source;
        }

        glLinkProgram(shader_program);
        GL_DEBUG_MESSAGES;

        GLint linked;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &linked);
        print_program_info_log(shader_program);
        if (!linked) throw Shader::Exception ("Error: Shaders not linked");
}


void Shader::bind()
{
        glUseProgram(shader_program);
}


void Shader::unbind()
{
        glUseProgram (0);
}


// ------------------
// Protected methods.
// ------------------

unsigned int Shader::get_shader_size(const string & filename)
{
        int fd;
        unsigned int count = 0;
        fd = open (filename.c_str (), O_RDONLY);
        if (fd != -1)
        {
                count = static_cast<unsigned int>(lseek(fd, 0, SEEK_END) + 1);
                close(fd);
        }
        else throw Shader::Exception(string("get_shader_size: bad Shader File Name") + filename);

        return count;
}


GLchar *Shader::read_shader_source(const string & shader_filename, unsigned int & shader_size)
{
        shader_size = get_shader_size(shader_filename);
        FILE *fh = fopen(shader_filename.c_str (), "r");

        if (!fh) throw Shader::Exception(string("read_shader_source : Bad Shader Filename ") + shader_filename);
    
        GLchar *shader_source = new GLchar[shader_size];
        fseek(fh, 0, SEEK_SET);
        int count = fread(shader_source, 1, shader_size, fh);
        shader_source[count] = '\0';
        fclose (fh);

        return shader_source;
}


GLint Shader::get_uni_loc(GLuint program, const GLchar *name)
{
        GLint loc = glGetUniformLocation (program, name);
        if (loc == -1) throw Shader::Exception (string ("No such uniform named") + string (name));
        GL_DEBUG_MESSAGES;
        return loc;
}

void Shader::print_shader_info_log(GLuint shader)
{
        int info_log_length = 0;
        int chars_written   = 0;
        GLchar *info_log;

        GL_DEBUG_MESSAGES;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
        GL_DEBUG_MESSAGES;

        if (info_log_length > 0)
        {
                info_log = new GLchar[info_log_length];
                if (info_log == NULL) throw Exception("Could not allocate InfoLog buffer\n");
                glGetShaderInfoLog(shader, info_log_length, &chars_written, info_log);
                string info_log_str(info_log);
                delete [] info_log;
                if (info_log_str.length () > 8 && info_log_str != "No errors.") throw Exception(info_log_str);
        }

        GL_DEBUG_MESSAGES;
}

void Shader::print_program_info_log(GLuint program)
{
        int info_log_length = 0;
        int chars_written   = 0;
        GLchar *info_log;

        GL_DEBUG_MESSAGES;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
        GL_DEBUG_MESSAGES;

        if (info_log_length > 0)
        {
                info_log = new GLchar[info_log_length];
                if (info_log == NULL) throw Exception ("Could not allocate InfoLog buffer\n");
                glGetProgramInfoLog(program, info_log_length, &chars_written, info_log);
                string info_log_str (info_log);
                delete [] info_log;
                if (info_log_str.length () > 8 && info_log_str != "No errors.") throw Exception (info_log_str);
        }

        GL_DEBUG_MESSAGES;
}


void Shader::compile_attach(GLuint & shader, GLenum type, const GLchar **source)
{
        GLint shader_compiled;
        shader = glCreateShader(type);
        glShaderSource(shader, 1, source, NULL);
        glCompileShader(shader);

        GL_DEBUG_MESSAGES;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);
        GL_DEBUG_MESSAGES;

        if (!shader_compiled)
        {
                cerr << *source << endl;
                print_shader_info_log(shader);
                throw Shader::Exception ("Error: shader not compiled");
        }

        glAttachShader(shader_program, shader);
}
