#pragma once
#include "RGLib/Mesh.hpp"
#include <iostream>
#include <fstream>
#define GLT_IMPLEMENTATION
#include <gltext.h>
#include <chrono>
namespace RGLib
{
	class World
	{

	public:
		World();
		void RenderWorld();

		void AddToWorld(glhelper::Mesh& mesh);

		//create GL queries for each mesh in world
		void CreateQueries();

		void Clean();
	private:
		std::vector<glhelper::Mesh*> worldObjects;
		std::vector<GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");
		GLTtext* fpsText;
		int frameCount;
		std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
		float frameDuration;
	};
}
