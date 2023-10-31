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
#include "RGLib/Texture.hpp"
#include "assimp/mesh.h"
#include "RGLib/Mesh.hpp"
#include "RGLib/Renderable.hpp"
#include "RGLib/RotateViewer.hpp"
#include <RGLib/Matrices.hpp>
#include <RGLib/World.hpp>
#include <opencv2/opencv.hpp>
#include <RGLib/Matrices.cpp>
#include "RGLib/FlyViewer.hpp"
namespace fs = std::filesystem;

int windowWidth = 1280;
int windowHeight = 720;
const Uint64 desiredFrametime = 16.6;

//Models
Model* model;


SDL_Event event;



int main()
{
	std::string path = "../shaders";
	for (const auto& entry : fs::directory_iterator(path))
		std::cout << entry.path() << std::endl;

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
	
	//glhelper::RotateViewer viewer(windowWidth, windowHeight);
	glhelper::FlyViewer viewer(windowWidth, windowHeight);

	//Shader
	glhelper::ShaderProgram lambertShader({ "..\\shaders\\Lambert.vert", "..\\shaders\\Lambert.frag" });
	//glProgramUniform4f(lambertShader.get(), lambertShader.uniformLoc("color"), 1.f, 1.f, 1.f, 1.f);

	Model* modelLoader = new Model();

	//Mesh
	glhelper::Mesh testMesh;
	testMesh.meshName = "TestMesh";

	Eigen::Matrix4f bunnyModelToWorld = Eigen::Matrix4f::Identity();
	bunnyModelToWorld(0, 0) = 0.2f;
	bunnyModelToWorld(1, 1) = 0.2f;
	bunnyModelToWorld(2, 2) = 0.2f;
	bunnyModelToWorld = makeTranslationMatrix(Eigen::Vector3f(0.f, -0.5f, 0.f)) * bunnyModelToWorld;

	modelLoader->loadFromFile("../models/stanford_bunny/scene.gltf", &testMesh);

	testMesh.loadTexture("../models/stanford_bunny/textures/Bunny_baseColor.png");
	//cv::Mat bunnyTextureImage = cv::imread("../models/stanford_bunny/textures/Bunny_baseColor.png");
	//cv::cvtColor(bunnyTextureImage, bunnyTextureImage, cv::COLOR_BGR2RGB);
	//glhelper::Texture bunnyTex(GL_TEXTURE_2D, GL_RGB8,bunnyTextureImage.cols, bunnyTextureImage.rows,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, bunnyTextureImage.data,GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR); //= new RGLib::Texture("../models/stanford_bunny/textures/Bunny_baseColor.png");
	//bunnyTex.genMipmap();
	//bunnyTextureImage.release();
	//testMesh.tex(bunnyTex);

	testMesh.modelToWorld(bunnyModelToWorld);
	testMesh.shaderProgram(&lambertShader);
	//testMesh.meshTex = &bunnyTex;

	//Lighthouse
	glhelper::Mesh lightHouse;
	lightHouse.meshName = "LightHouse";


	Eigen::Matrix4f lighthouseModelToWorld = Eigen::Matrix4f::Identity();
	lighthouseModelToWorld = makeIdentityMatrix(5);
	//lighthouseModelToWorld(0, 0) = 1.f;
	//lighthouseModelToWorld(1, 1) = 1.f;
	//lighthouseModelToWorld(2, 2) = 1.f;

	lighthouseModelToWorld = makeTranslationMatrix(Eigen::Vector3f(10.f, -0.5f, 0.f)) * makeRotationMatrix(-90, 0, 180) * lighthouseModelToWorld;

	modelLoader->loadFromFile("../models/lighthouse2/source/Lighthouse.fbx", &lightHouse);
	
	lightHouse.loadTexture("../models/lighthouse2/textures/Base_color.png");
	//cv::Mat lightHouseTextureImage = cv::imread("../models/lighthouse2/textures/Base_color.png");
	//cv::cvtColor(lightHouseTextureImage, lightHouseTextureImage, cv::COLOR_BGR2RGB);
	//glhelper::Texture lighthouseTex(GL_TEXTURE_2D, GL_RGB8, lightHouseTextureImage.cols, lightHouseTextureImage.rows,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, lightHouseTextureImage.data, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR); //= new RGLib::Texture("../models/stanford_bunny/textures/Bunny_baseColor.png");
	//lighthouseTex.genMipmap();
	//lightHouseTextureImage.release();

	lightHouse.modelToWorld(lighthouseModelToWorld);
	lightHouse.shaderProgram(&lambertShader);
	//lightHouse.meshTex = &lighthouseTex;

	glhelper::Mesh rock;
	rock.meshName = "Rock";

	Eigen::Matrix4f rockModelToWorld = Eigen::Matrix4f::Identity();
	rockModelToWorld = makeIdentityMatrix(0.05f);
	/*rockModelToWorld(0, 0) = 0.02f;
	rockModelToWorld(1, 1) = 0.02f;
	rockModelToWorld(2, 2) = 0.02f;*/
	rockModelToWorld = makeTranslationMatrix(Eigen::Vector3f(0, -0.5f, 0)) * makeScaleMatrix(1) * rockModelToWorld;
	modelLoader->loadFromFile("../models/obj-nat-rock/source/nat-rock.obj", &rock);
	rock.loadTexture("../models/obj-nat-rock/textures/nat-rock-diff.jpeg");
	rock.shaderProgram(&lambertShader);
	rock.modelToWorld(rockModelToWorld);

	glhelper::Mesh rock2;
	rock2.meshName = "rock 2";
	Eigen::Matrix4f rock2ModelToWorld = Eigen::Matrix4f::Identity();
	rock2ModelToWorld = makeIdentityMatrix(0.01f);

	rock2ModelToWorld = makeTranslationMatrix(Eigen::Vector3f(-5, -0.5f, 0)) * rock2ModelToWorld;

	rock2.loadTexture("../models/river-rock/textures/RiverRock_BaseColor.png");
	modelLoader->loadFromFile("../models/river-rock/source/River_Rock.fbx", &rock2);
	rock2.shaderProgram(&lambertShader);
	rock2.modelToWorld(rock2ModelToWorld);

	std::vector<glhelper::Renderable*> scene{ &testMesh, &lightHouse, &rock };

	RGLib::World* Worldscene = new RGLib::World;
	Worldscene->AddToWorld(testMesh);
	Worldscene->AddToWorld(lightHouse);
	Worldscene->AddToWorld(rock);
	Worldscene->AddToWorld(rock2);
	Worldscene->CreateQueries();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	bool running = true;
		while (running)
		{
			Uint64 frameStartTime = SDL_GetTicks64();



			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					running = false;
					//Worldscene->Clean();
					delete Worldscene;

					return 0;
				}
				else
				{
					viewer.processEvent(event);
					viewer.update();
				}
			}
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, windowWidth, windowHeight);

			Worldscene->RenderWorld();


			SDL_GL_SwapWindow(window);

			Uint64 elapsedFrameTime = SDL_GetTicks64() - frameStartTime;
			if (elapsedFrameTime < desiredFrametime) {
				SDL_Delay(desiredFrametime - elapsedFrameTime);
			}
		};

		//Clean up
			
	return 0;
}

void Clean()
{

}