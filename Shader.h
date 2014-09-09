#pragma once

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>


class Shader
{

        public:
    
                class Exception
                {
                        public:

                                inline Exception(const std::string &msg) : _msg ("Shader Exception: " + msg){}
                                inline const std::string & msg () const { return _msg; }

                        protected:

                                std::string _msg;
                };
    
                Shader ();
                virtual ~Shader();

                inline bool has_vertex_shader()   const { return (vertex_shader_size > 0);   }
                inline bool has_fragment_shader() const { return (fragment_shader_size > 0); }

                inline GLuint get_shader_program()  { return shader_program;  }
                inline GLuint get_vertex_shader()   { return vertex_shader;   }
                inline GLuint get_fragment_shader() { return fragment_shader; }

                void loadFromFile (const std::string & vertexShaderFilename,
                                   const std::string & fragmentShaderFilename);

                inline void loadFromFile (const std::string & vertexShaderFilename)
                {
                    loadFromFile(vertexShaderFilename, "");
                }

                void bind();
                void unbind();
	
	protected:

		GLchar *read_shader_source(const std::string &shaderFilename, unsigned int &shaderSize);

		GLint get_uni_loc(GLuint program, const GLchar *name);
		inline GLint get_uni_loc(const GLchar*name) { return get_uni_loc(shader_program, name); }

		void compile_attach(GLuint & shader, GLenum type, const GLchar ** source);

		static void print_shader_info_log  (GLuint shader );
		static void print_program_info_log (GLuint program);

		// Returns the size in bytes of the shader fileName. If an error occurred, it returns -1.
		static unsigned int get_shader_size(const std::string & shader_filename);
	
	private:

		GLuint  shader_program,
			vertex_shader,
			fragment_shader;

		unsigned int vertex_shader_size,
			     fragment_shader_size;
};
