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
	/*! \brief Used to store mesh reference alongside normal map and performance query information
	*
	*/
	class WorldObject
	{
	public:
		WorldObject(glhelper::Mesh& mesh)
		{
			objMesh = &mesh;
			glGenQueries(1, &query);
			//world->AddWorldObject(*this);
		};
		WorldObject()
		{};
		~WorldObject()
		{
			glDeleteTextures(1, &normalMap);
			glDeleteQueries(1, &query);
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

		GLuint GetQuery()
		{
			return query;
		}

		bool IsQueryQueued()
		{
			return queryQueued;
		}

		void SetQueryQueued(bool _true)
		{
			queryQueued = _true;
		}

		void SetInReflection(bool _true)
		{
			inReflection = _true;
		}
		bool IsInReflection()
		{
			return inReflection;
		}

	private:
		glhelper::Mesh* objMesh;
		GLuint normalMap;
		GLuint query;
		bool queryQueued = false;
		bool inReflection;

	};
	/*! \brief The world class is responsible for world objects and performance data
* 
* The world class handles all the objects within the game scene and renders them accordingly
* The world class is also responsible for outputting performance metrics
* To handle performance the world class works with GL Queries for models these are stored within the WorldObject class
*/

	class World
	{

	public:
	
		World();/*!< class constructor */
		void RenderWorldObjects(); /*!< Renders all objects within the worldObjects array */
		void RenderWorldObjects(glhelper::ShaderProgram& shader);/*!< Same as RenderWorldObjects, with custom shader input, used in depth of field */
		void RenderReflectedObjects(); /*!< Renders all objects within the worldObjects array upside down for reflections */
		void RenderGUI(); /*!< Renders statically set text UI elements*/
		void CreateShadowMaps(); /*!< Unused function, part of unsuccessful shadow map conversion efforts*/
		void RenderShadowMaps(); /*!< Unused function, part of unsuccessful shadow map conversion efforts*/
		void AddToWorld(glhelper::Mesh& mesh); /*!< Adds a mesh to the worldMeshes array, depreciated in favour of AddWorldObject*/
		void RecordQueries(); /*!< Saves all queries from WorldObjects & queries dictionary to file ready for writing, Call at end of game loop*/
		void AddWorldObject(glhelper::Mesh& mesh, bool reflect = true); /*!< Adds a mesh to the WorldObject array, for meshes WITHOUT normal maps*/
		void AddWorldObject(glhelper::Mesh& mesh, GLuint normalMap, bool reflect = true); /*< Adds a mesh to the WorldObject array, for meshes WITH normal maps*/
		void AddWorldLight(RGLib::Light light); /*!< Add a world light into the scene to be used in shadow-maps which were not properly converted making this function redundant*/
		
		void CreateQueries();/*!< create GL queries for each mesh in world. \n Needs to be called before rendering objects */

		void Clean();

		void ClearWorld();

		std::map< std::string, GLuint> GetQueries()
		{
			return queries;
		}

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
		//std::vector<GLuint> queries;
		std::map< std::string, GLuint> queries;
		std::ofstream dataFile; //("renderData.csv");
		GLTtext* fpsText;
		GLTtext* frameCountText;

		int frameCount;
		int frameSync;
		std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
		float frameDuration;
		std::vector< WorldObject*> worldObjects;

		bool queriesAvailible = true;

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
