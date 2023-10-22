#pragma once
#include <GL/glew.h>
#include <string>

namespace RGLib {

	class ShaderUtils
	{
	public:
		bool loadShaderSourceCode(std::string v_filename, std::string f_filename);
		bool compileProgram(const std::string& v_shader_sourcecode, const std::string& f_shader_sourcecode, GLuint* programId);


		//void setVShaderSource(std::string v_shader)
		//{
		//	v_shader_source = v_shader;
		//}

		//void setFShaderSource(std::string f_shader)
		//{
		//	f_shader_source = f_shader;
		//}

		std::string v_shader_source;
		std::string f_shader_source;
	};

}
