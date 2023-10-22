#include "ShaderUtils.h"
#include <iostream>
#include <fstream>

namespace RGLib {

	ShaderUtils::ShaderUtils()
	{
		// This is a helper function that allows us to see
	// shader compiler error messages should our shaders not compile okay
		void _displayShaderCompilerError(GLuint shaderId) {
			// First, get the length of the error message string
			GLint MsgLen = 0;

			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

			// Only display the message is more than 1 character was returned
			if (MsgLen > 1) {
				// Create a character buffer to store the error message characters
				GLchar* Msg = new GLchar[MsgLen + 1];

				// Get the error message characters from the OpenGL internal log
				// into the Msg buffer
				glGetShaderInfoLog(shaderId, MsgLen, nullptr, Msg);

				// Display the error message so we can see what the problem
				// was with the shader
				std::cerr << "Error compiling shader" << Msg << std::endl;

				// Release the memory allocated to the string
				delete[] Msg;
			}
		}

		// Read shader source into a string for compiling later
		// Can't claim credit for the reading bit as is a common
		// C++ idiom for reading text from a file into memory
		bool loadShaderSourceCode(std::string v_filename, std::string f_filename)
		{
			// Open an input file stream.  Defaults to reading character data
			std::ifstream ifs(v_filename);

			if (!ifs.is_open()) {
				std::cerr << "Problem opening file: " << v_filename << " Check file is in the directory" << std::endl;
				// Return a default string indicating no shader was loaded
				// Will result in a compiler error.  Not sophisticated and
				// needs refinement
				return false;
			}

			// Use an iterator to read characters into string
			std::string shader_source((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));

			
			v_shader_source = shader_source;

			ifs.open(f_filename);

			if (!ifs.is_open()) {
				std::cerr << "Problem opening file: " << f_filename << " Check file is in the directory" << std::endl;

				// Return a default string indicating no shader was loaded
				// Will result in a compiler error.  Not sophisticated and
				// needs refinement
				return false;
			}

			// Use an iterator to read characters into string
			std::string shader_source2((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));

			f_shader_source = shader_source2;


			// Return the string
			return true;
		}

		bool compileProgram(GLuint * programId) {
			// Create the vertex shader first.
			// Order doesn't matter but shaders must be created and compiled before
			// attaching to program
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

			// Due to the unique way OpenGL handles shader source, OpenGL expects
			// an array of strings.  In this case, create an array of the
			// loaded source code strings and pass to compileProgram for compilation
			//const GLchar* v_source_array[] = { v_shader_sourcecode.c_str() };

			// Copy the source to OpenGL ready for compilation
			//glShaderSource(vertexShader, 1, v_shader_sourcecode, nullptr);

			// Compile the code
			glCompileShader(vertexShader);

			// Check for compiler errors
			// Presume shader didn't compile
			GLint isShaderCompiledOK = GL_FALSE;

			// Get the compile status from OpenGL
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

			// Has the shader failed to compile?
			if (isShaderCompiledOK != GL_TRUE) {
				// Yes, so display an error message
				std::cerr << "Unable to compile vertex shader" << std::endl;

				_displayShaderCompilerError(vertexShader);

				return false;
			}

			// Do the same for the fragment shader
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			// Due to the unique way OpenGL handles shader source, OpenGL expects
			// an array of strings.  In this case, create an array of the
			// loaded source code strings and pass to compileProgram for compilation
			const GLchar* f_source_array[] = { f_shader_source.c_str() };

			// Transfer the shader code
			glShaderSource(fragmentShader, 1, f_source_array, NULL);

			// Compile it
			glCompileShader(fragmentShader);

			// Check for errors.  Code is same as above of getting status
			// and displaying error message, if necessary
			isShaderCompiledOK = GL_FALSE;
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

			if (isShaderCompiledOK != GL_TRUE) {
				std::cerr << "Unable to compile fragment shader" << std::endl;

				_displayShaderCompilerError(fragmentShader);

				return false;
			}

			// Create the program object
			*programId = glCreateProgram();

			// Attach shaders to the program object
			glAttachShader(*programId, vertexShader);
			glAttachShader(*programId, fragmentShader);

			// Now link the program to create an executable program we
			// and use to render the object
			// Program executable will exist in graphics memory
			glLinkProgram(*programId);

			// Check for linking errors
			GLint isProgramLinked = GL_FALSE;
			glGetProgramiv(*programId, GL_LINK_STATUS, &isProgramLinked);
			if (isProgramLinked != GL_TRUE) {
				std::cerr << "Failed to link program" << std::endl;

				return false;
			}

			// Got this far so must be okay, return true
			return true;
		}
	}
	

}