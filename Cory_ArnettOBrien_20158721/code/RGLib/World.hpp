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

		//void AddWorldObject(RGLib::WorldObject& object);

		//create GL queries for each mesh in world
		void CreateQueries();

		void Clean();

		void ClearWorld();
	private:
		std::vector<glhelper::Mesh*> /*std::vector<WorldObject*>**/ worldObjects;
		std::vector<GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");
		GLTtext* fpsText;
		int frameCount;
		std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
		float frameDuration;
	};

	class WorldObject
	{
	public:
		WorldObject(glhelper::Mesh &mesh)
		{
			objMesh = &mesh;
			//world->AddWorldObject(*this);
		};

		std::string getName()
		{
			return objMesh->meshName;
		};

		glhelper::Mesh* getMesh()
		{
			return objMesh;
		}
		
	private:
		glhelper::Mesh* objMesh;
		GLuint query;
	};
}
