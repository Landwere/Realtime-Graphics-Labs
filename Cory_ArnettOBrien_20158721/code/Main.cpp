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
#include "RGLib/Texture.h"
#include "assimp/mesh.h"
#include "RGLib/Mesh.hpp"
#include "RGLib/Renderable.hpp"
#include "RGLib/RotateViewer.hpp"
#include <RGLib/Matrices.hpp>
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
	
	glhelper::RotateViewer viewer(windowWidth, windowHeight);

	Model* modelLoader = new Model();

	glhelper::Mesh testMesh;
	std::vector<glhelper::Renderable*> scene{ &testMesh };

	Eigen::Matrix4f bunnyModelToWorld = Eigen::Matrix4f::Identity();
	bunnyModelToWorld(0, 0) = 0.2f;
	bunnyModelToWorld(1, 1) = 0.2f;
	bunnyModelToWorld(2, 2) = 0.2f;
	bunnyModelToWorld = makeTranslationMatrix(Eigen::Vector3f(0.f, -0.5f, 0.f)) * bunnyModelToWorld;

	modelLoader->loadFromFile("../models/stanford_bunny/scene.gltf", &testMesh);

	glhelper::ShaderProgram lambertShader({ "..\\shaders\\FixedColor.vert", "..\\shaders\\FixedColor.frag" });
	glProgramUniform4f(lambertShader.get(), lambertShader.uniformLoc("color"), 1.f, 1.f, 1.f, 1.f);

	testMesh.shaderProgram(&lambertShader);
	testMesh.modelToWorld(bunnyModelToWorld);
	//std::string v_shader = RGLib::loadShaderSourceCode("../shaders\\lambert.vert");
	//std::string f_shader = RGLib::loadShaderSourceCode("../shaders\\lambert.frag");

	GLuint shaderProgram;
	GLuint testShader = glCreateShader(GL_VERTEX_SHADER);
	//RGLib::compileProgram(v_shader, f_shader, &shaderProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);

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
					viewer.processEvent(event);
				}
			}
			testMesh.render();
			//modr->draw(cam, model, tex);
		};

	return 0;
}
