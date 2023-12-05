#pragma once
#include "RGLib/Mesh.hpp"
#include <iostream>
#include <fstream>
#define GLT_IMPLEMENTATION
#include <gltext.h>
#include <chrono>
#include "RGLib/json.hpp"
#include "RGLib/Texture.hpp"
namespace RGLib
{
	class WorldObject
	{
	public:
		WorldObject(glhelper::Mesh& mesh)
		{
			objMesh = &mesh;
			//world->AddWorldObject(*this);
		};
		WorldObject()
		{};
		std::string getName()
		{
			return objMesh->meshName;
		};

		glhelper::Mesh* getMesh()
		{
			return objMesh;
		}

		void SetNormal(GLuint nMap)
		{
			normalMap = nMap;
		}

		GLuint GetNormal()
		{
			return normalMap;
		}

	private:
		glhelper::Mesh* objMesh;
		GLuint normalMap;
		GLuint query;
	};

	class World
	{

	public:
		World();
		void RenderWorld();

		void AddToWorld(glhelper::Mesh& mesh);

		void AddWorldObject(glhelper::Mesh& mesh);
		void AddWorldObject(glhelper::Mesh& mesh, GLuint normalMap);

		//create GL queries for each mesh in world
		void CreateQueries();

		void Clean();

		void ClearWorld();

		nlohmann::json* models;
		/*std::map<std::string, glhelper::Mesh>* meshes;
		std::map<std::string, glhelper::ShaderProgram>* shaders;
		std::map<std::string, glhelper::Texture>* textures;*/

	private:
		std::vector<glhelper::Mesh*> /*std::vector<WorldObject*>**/ worldMeshes;
		std::vector<GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");
		GLTtext* fpsText;
		int frameCount;
		std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
		float frameDuration;
		std::vector< WorldObject*> worldObjects;
	};

	
}
