#pragma once
#include "RGLib/Mesh.hpp"
#include <iostream>
#include <fstream>
#define GLT_IMPLEMENTATION
#include <gltext.h>
#include <chrono>
#include "RGLib/json.hpp"
#include "RGLib/Texture.hpp"
#include <RGLib/Matrices.hpp>
#include "Light.hpp"
#include "RGLib/ShadowMap.hpp"

//!  World
/*! 
* The world class handles all the objects within the game scene and renders them accordingly 
* The world class is also responsible for outputting performance metrics
*/

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
		~WorldObject()
		{
			glDeleteTextures(1, &normalMap);
			delete(&objMesh->meshTex);
		}

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
	/*! Text world class */

	class World
	{

	public:
		/** constructor for calss
		*/
		World();
		void RenderWorldObjects();
		void RenderWorldObjects(glhelper::ShaderProgram& shader);
		void RenderGUI();
		void CreateShadowMaps();
		void RenderShadowMaps();
		void AddToWorld(glhelper::Mesh& mesh);

		void AddWorldObject(glhelper::Mesh& mesh);
		void AddWorldObject(glhelper::Mesh& mesh, GLuint normalMap);
		void AddWorldLight(RGLib::Light light);
		//create GL queries for each mesh in world
		void CreateQueries();

		void Clean();

		void ClearWorld();

		nlohmann::json* models;
		/*std::map<std::string, glhelper::Mesh>* meshes;
		std::map<std::string, glhelper::ShaderProgram>* shaders;
		std::map<std::string, glhelper::Texture>* textures;*/

		void SetShadowMapShaders(glhelper::ShaderProgram &sMShader, glhelper::ShaderProgram &sCMShader);

		glhelper::Mesh* ground;
	private:
		std::vector<glhelper::Mesh*> shadowRec;

		RGLib::ShadowMap* shadowMap;
		std::vector<glhelper::Mesh*> /*std::vector<WorldObject*>**/ worldMeshes;
		std::vector<GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");
		GLTtext* fpsText;
		int frameCount;
		std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
		float frameDuration;
		std::vector< WorldObject*> worldObjects;


		Eigen::Matrix4f flipMatrix;

		std::vector<RGLib::Light*> worldLights;

		//shadow
		float shadowMapSize;
		const float shadowMapNear = 1.f, shadowMapFar = 1000.f;
		float shadowMapBias = 1.0f;
		GLuint cubeMapTexture;
		Eigen::Matrix4f cubemapPerspective;
		glhelper::ShaderProgram* shadowMappedShader;
		glhelper::ShaderProgram* shadowCubeMapShader;
		GLuint frameBuffer;

	};

	
}
