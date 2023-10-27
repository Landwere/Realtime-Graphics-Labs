#pragma once
#include "RGLib/Mesh.hpp"
#include <iostream>
#include <fstream>
namespace RGLib
{
	class World
	{

	public:
		void RenderWorld();

		void AddToWorld(glhelper::Mesh& mesh);

		void CreateQueries();

	private:
		std::vector<glhelper::Mesh*> worldObjects;
		std::vector<GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");

	};
}
