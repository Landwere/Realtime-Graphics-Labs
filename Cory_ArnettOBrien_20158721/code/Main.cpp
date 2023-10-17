#define SDL_MAIN_HANDLED
#include <GL/glew.h>

#include <SDL.h>

#include <iostream>
#include "RGLib/modelLoader.hpp"
#include "RGLib/ShaderProgram.hpp"
#include "RGLib/ShaderUtils.h"

int windowWidth = 1280;
int windowHeight = 720;

//Models
Model* model;


SDL_Event event;


int main()
{
	


	model = new Model();
	bool result = model->loadFromFile(".\\models\\sphere.obj");


	//Set up SDL window
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);


	SDL_Window* window;
	window = SDL_CreateWindow("Realtime Graphics", 50, 50, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum gStatus = glewInit();
	if (GLEW_OK != gStatus)
	{
		std::cerr << "Problem starting glew " << glewGetErrorString(gStatus) << std::endl;
	}

//	RGLib::ShaderProgram lambertShader({ ".\\shaders\\lambert.vert", ".\\shaders\\lambert.frag" });
	std::string v_shader = RGLib::loadShaderSourceCode("../shaders/lambert.vert");
	std::string f_shader = RGLib::loadShaderSourceCode("../shaders/lambert.frag");

	GLuint shaderProgram;
	GLuint testShader = glCreateShader(GL_VERTEX_SHADER);
	RGLib::compileProgram(v_shader, f_shader, &shaderProgram);

	bool running = true;
		while (running)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					running = false;
				}
				else
				{
					
				}
			}
		};

	return 0;
}