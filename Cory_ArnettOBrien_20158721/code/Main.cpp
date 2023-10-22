#define SDL_MAIN_HANDLED
#include <GL/glew.h>

#include <SDL.h>
#include <string>
#include <filesystem>
#include <iostream>
#include "RGLib/modelLoader.hpp"
#include "RGLib/ShaderProgram.hpp"
//#include "RGLib/ShaderUtils.h"
#include <glm/glm.hpp>
#include "RGLib/Camera.h"
#include "RGLib/ModelRenderer.h"
#include "RGLib/Texture.h"

namespace fs = std::filesystem;

int windowWidth = 1280;
int windowHeight = 720;

//Models
Model* model;


SDL_Event event;



int main()
{
	std::string path = "../shaders";
	for (const auto& entry : fs::directory_iterator(path))
		std::cout << entry.path() << std::endl;


	model = new Model();
	bool result = model->loadFromFile(".\\models\\sphere.obj");

	RGLib::ModelRenderer* modr = new RGLib::ModelRenderer();
	RGLib::Texture* tex = new RGLib::Texture("");

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


	RGLib::Camera* cam = new RGLib::Camera(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),//look at
		glm::vec3(0.0f, 1.0f, 0.0f),//up direction
		90.0f, windowWidth / windowHeight, 0.1f, 9000.0f); // fov, aspect ratio based on window dimensions
	



//	RGLib::ShaderProgram lambertShader({ ".\\shaders\\lambert.vert", ".\\shaders\\lambert.frag" });
	//std::string v_shader = RGLib::loadShaderSourceCode("../shaders\\lambert.vert");
	//std::string f_shader = RGLib::loadShaderSourceCode("../shaders\\lambert.frag");

	GLuint shaderProgram;
	GLuint testShader = glCreateShader(GL_VERTEX_SHADER);
	//RGLib::compileProgram(v_shader, f_shader, &shaderProgram);

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
			//modr->draw(cam, model, tex);
		};

	return 0;
}
