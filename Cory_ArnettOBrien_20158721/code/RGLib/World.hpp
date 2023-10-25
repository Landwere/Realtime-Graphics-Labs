#pragma once
#include "RGLib/Mesh.hpp"

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
		std::vector<GLuint*> queries;
	};
}
