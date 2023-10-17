#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "RGLib/modelLoader.hpp"
#include "RGLib/ShaderProgram.hpp"


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
	SDL_Window* window;
	window = SDL_CreateWindow("Realtime Graphics", 50, 50, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

	RGLib::ShaderProgram lambertShader({ "../shaders/lambert.vert", "../shaders/lambert.frag" });

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