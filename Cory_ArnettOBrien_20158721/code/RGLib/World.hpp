#pragma once
#include "RGLib/Mesh.hpp"
#include <iostream>
#include <fstream>
#define GLT_IMPLEMENTATION
#include <gltext.h>
namespace RGLib
{
	class World
	{

	public:
		World();
		void RenderWorld();

		void AddToWorld(glhelper::Mesh& mesh);

		void CreateQueries();

	private:
		std::vector<glhelper::Mesh*> worldObjects;
		std::vector<GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");
		GLTtext* fpsText;
		int frameCount;
	};
}
