#pragma once
#include <GL/glew.h>
#include <string>

namespace RGLib {
	std::string loadShaderSourceCode(std::string filename);
	bool compileProgram(std::string& v_shader_sourcecode, const std::string& f_shader_sourcecode, GLuint* programId);
}
