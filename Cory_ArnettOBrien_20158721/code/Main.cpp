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
#include <RGLib/Light.hpp>
#include <opencv2/opencv.hpp>
#include <RGLib/Matrices.cpp>
#include "RGLib/FlyViewer.hpp"
#include "RGLib/json.hpp"
#include <RGLib/FrameBuffer.h>
#include <random>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <bullet/btBulletDynamicsCommon.h>

/*! \mainpage Cory Arnett-O'Brien, Realtime computer graphics
	This is the Doxygen documentation for the realtime computer graphics program created by Cory Arnett-O'Brien for the CMP7172 assignment

*/


///! <summary>
/// Summary test
/// </summary>

namespace fs = std::filesystem;

int windowWidth = 1280;
int windowHeight = 720;

//frametime to lock application to in milliseconds
const Uint64 desiredFrametime = 16.6;

//Models
Model* model;


SDL_Event event;
float theta = 0.0f;
Eigen::Vector4f albedo(0.1f, 0.6f, 0.6f, 1.f);
float specularExponent = 60.f, specularIntensity = 0.05f;
//float lightIntensity =  30.f;
float fallOffExponent = 2.0f;
Eigen::Vector3f worldLightDir(40,90, 0);
float worldLightIntensity = 0.5f;
float lightWidth = 0.01f;
int sampleRadius = 1;

//shadow
const float shadowMapNear = 1.f, shadowMapFar = 1000.f;
float shadowMapBias = 1.0f; /*! shadow map depth distance bias to prevent artefacts*/
const int shadowMapSize = 1024; /*!<Shadow map resolution*/

nlohmann::json models;
std::map<std::string, glhelper::Mesh> meshes;
std::map<std::string, glhelper::ShaderProgram> shaders;
std::map<std::string, glhelper::Texture> textures;

RGLib::World* Worldscene;

const int nRParticles = 20000;
float ringMinRadius = 5.f;
float ringMaxRadius = 100.f;
float gridWidth = 6;
float particleInitialVelocity = 0.0f;
float particleMass = 1.f;
float gravitationalConstant = -1.f;
float rainParticleSize = 0.03f;

void Clean();

//Quad renderer from https://learnopengl.com/Advanced-Lighting/HDR
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
//end source
//TODO Remove loadSpotMeshFunction ONLY USED FOR TESTING source Moodle
void loadSpotMesh(glhelper::Mesh* mesh)
{
	// ----- Your code here -----
	// Change the code here to generate tangents and bitangents when loading the mesh, and add
	// them to the glhelper::Mesh instance.
	// You can use the Mesh::tangent() and Mesh::bitangent() methods to do this, they work
	// the same way as the Mesh::norm() method.

	Assimp::Importer importer;
	importer.ReadFile("../models/spot/spot_triangulated.obj", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	const aiScene* aiscene = importer.GetScene();
	const aiMesh* aimesh = aiscene->mMeshes[0];

	std::vector<Eigen::Vector3f> verts(aimesh->mNumVertices);
	std::vector<Eigen::Vector3f> norms(aimesh->mNumVertices);
	std::vector<Eigen::Vector2f> uvs(aimesh->mNumVertices);
	std::vector<Eigen::Vector3f> tangents(aimesh->mNumVertices);
	std::vector<Eigen::Vector3f> biTangents(aimesh->mNumVertices);


	std::vector<GLuint> elems(aimesh->mNumFaces * 3);

	memcpy(biTangents.data(), aimesh->mBitangents, aimesh->mNumVertices * sizeof(aiVector3D));
	memcpy(tangents.data(), aimesh->mTangents, aimesh->mNumVertices * sizeof(aiVector3D));
	memcpy(verts.data(), aimesh->mVertices, aimesh->mNumVertices * sizeof(aiVector3D));
	memcpy(norms.data(), aimesh->mNormals, aimesh->mNumVertices * sizeof(aiVector3D));
	for (size_t v = 0; v < aimesh->mNumVertices; ++v) {
		uvs[v][0] = aimesh->mTextureCoords[0][v].x;
		uvs[v][1] = 1.f - aimesh->mTextureCoords[0][v].y;
	}
	for (size_t f = 0; f < aimesh->mNumFaces; ++f) {
		for (size_t i = 0; i < 3; ++i) {
			elems[f * 3 + i] = aimesh->mFaces[f].mIndices[i];
		}
	}

	mesh->tangent(tangents);
	mesh->bitangent(biTangents);

	mesh->vert(verts);
	mesh->norm(norms);
	//mesh->elems(elems);
	mesh->tex(uvs);
}
//end source
void ReloadConfig()
{
	Model* modelLoader = new Model();

	//Code from config lab David Walton

	nlohmann::json data;
	try {
		std::ifstream jsonFile("../config/config.json");
		data = nlohmann::json::parse(jsonFile);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		exit(1);
	}

	// Load all the meshes
	for (auto& mesh : data["meshes"]) {
		std::cout << mesh["name"] << " " << mesh["filename"] << std::endl;
		std::string meshName = mesh["name"];
		meshes.emplace(std::piecewise_construct, std::forward_as_tuple(meshName), std::forward_as_tuple());
		modelLoader->loadFromFile(mesh["filename"], &(meshes[mesh["name"]])); //loadMesh(&(meshes[mesh["name"]]), mesh["filename"]);
	}

	// Load all the shaders
	for (auto& shader : data["shaders"]) {
		std::vector<std::string> sourceFilenames;
		for (auto& filename : shader["filenames"]) {
			sourceFilenames.push_back(filename);
		}

		std::string shaderName = shader["name"];
		shaders.emplace(shaderName, sourceFilenames);

	}

	// Load all the textures
	for (auto& texture : data["textures"]) {
		std::string textureName = texture["name"];
		cv::Mat image = cv::imread(texture["filename"]);
		textures.emplace(textureName, glhelper::Texture{ GL_TEXTURE_2D, GL_RGB8, (size_t)image.cols, (size_t)image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data });
	}

	models = data["models"];

	//end refference 
	//Worldscene->ClearWorld();

	//for (auto& model : models) {
	//	glhelper::Mesh& mesh = meshes.at(model["mesh"]);
	//	glhelper::ShaderProgram& shader = shaders.at(model["shader"]);
	//	glhelper::Texture& texture = textures.at(model["texture"]);
	//	texture.bindToImageUnit(0);
	//	glProgramUniform1i(shader.get(), shader.uniformLoc("tex"), 0);
	//	mesh.shaderProgram(&shader);
	//	//mesh.meshTex = std::make_unique< glhelper::Texture>(GL_TEXTURE_2D, GL_RGB8, texture.height(), texture.width(),
	//	//	0, GL_RGB, GL_UNSIGNED_BYTE, texture.getData(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	//	Eigen::Vector3f position(model["position"][0], model["position"][1], model["position"][2]);
	//	mesh.modelToWorld(makeTranslationMatrix(position));
	//	//mesh.setTexture(texture);
	//	Worldscene->AddToWorld(mesh);
	//}
}
//create texture function from Texture lab by David Walton 
GLuint createTexture(const cv::Mat& image)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);

	glGenerateTextureMipmap(texture);

	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// This isn't strictly necessary as GL_REPEAT is the default mode.
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texture;
}
//create texture function from Texture lab by David Walton adjusted to take filename input
GLuint createTexture(cv::String filename)
{
	cv::Mat image = cv::imread(filename);
	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);

	glGenerateTextureMipmap(texture);

	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// This isn't strictly necessary as GL_REPEAT is the default mode.
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	image.release();
	return texture;
}
//end source
int main()
{
	RGLib::Light* lampLight = new RGLib::Light(100, Eigen::Vector3f(-0.1f, 7.f, 0.f)); /*Light from lighthouse lamp*/

	//lightPos = Eigen::Vector3f(-0.1f, 7.f, 0.f);
	//lightPos = Eigen::Vector3f(0, 6, -5);
	worldLightDir.normalize();


	//print out all shaders in path (used for debugging)
	std::string path = "../shaders";
	for (const auto& entry : fs::directory_iterator(path))
		std::cout << entry.path() << std::endl;

	//Set up SDL window
	SDL_Init(SDL_INIT_VIDEO);
	//Set Open-GL attributes 
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	// Turns on 4x MSAA
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);


	SDL_Window* window;
	window = SDL_CreateWindow("Realtime Graphics", 50, 50, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum gStatus = glewInit();
	if (GLEW_OK != gStatus)
	{
		std::cerr << "Problem starting glew " << glewGetErrorString(gStatus) << std::endl;
	}
	//Enable Vsync
	SDL_GL_SetSwapInterval(1);


	RGLib::Camera* cam = new RGLib::Camera(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),//look at
		glm::vec3(0.0f, 1.0f, 0.0f),//up direction
		90.0f, windowWidth / windowHeight, 0.4f, 900.0f); // fov, aspect ratio based on window dimensions

	glhelper::FlyViewer viewer(windowWidth, windowHeight);

	//Compile shader programs
	glhelper::ShaderProgram lambertShader({ "..\\shaders\\Lambert.vert", "..\\shaders\\Lambert.frag" });
	glhelper::ShaderProgram blinnPhongShader({ "..\\shaders\\BlinnPhong.vert", "..\\shaders\\BlinnPhong.frag" });
	glhelper::ShaderProgram shadowCubeMapShader({ "..\\shaders\\ShadowCubeMap.vert", "..\\shaders\\ShadowCubeMap.frag" });
	glhelper::ShaderProgram shadowMappedShader({ "..\\shaders\\ShadowMapped.vert", "..\\shaders\\ShadowMapped.frag" });
	glhelper::ShaderProgram billboardParticleShader({ "../shaders/BillboardParticle.vert", "../shaders/BillboardParticle.geom", "../shaders/BillboardParticle.frag" });
	glhelper::ShaderProgram RainPhysicsShader({ "../shaders/RainParticle.comp" });
	glhelper::ShaderProgram NormalShader({ "..\\shaders\\NormalShader.vert", "..\\shaders\\NormalShader.frag" });
	glhelper::ShaderProgram HDRShader({ "..\\shaders\\HDRShader.vert", "..\\shaders\\HDRShader.frag" });
	glhelper::ShaderProgram waterShader({ "..\\shaders\\Water.vert", "..\\shaders\\Water.frag" });
	glhelper::ShaderProgram DOFShader({ "..\\shaders\\DepthTest.vert", "..\\shaders\\DepthTest.frag" });

	//set up blinnPhong shader uniforms
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("specularExponent"), specularExponent);
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("specularIntensity"), specularIntensity);
	glProgramUniform3f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightIntensity"), lampLight->Intensity());
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("falloffExponent"), fallOffExponent);
	glProgramUniform3fv(blinnPhongShader.get(), blinnPhongShader.uniformLoc("worldLightDir"), 1, worldLightDir.data());
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("worldLightInt"), worldLightIntensity);
	//set up NormalShader shader uniforms
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("specularExponent"), specularExponent);
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("specularIntensity"), specularIntensity);
	glProgramUniform3f(NormalShader.get(), NormalShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("lightIntensity"), lampLight->Intensity());
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("falloffExponent"), fallOffExponent);
	glProgramUniform3fv(NormalShader.get(), NormalShader.uniformLoc("worldLightDir"), 1, worldLightDir.data());
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("worldLightInt"), worldLightIntensity);
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("albedoTex"), 0);
	glProgramUniform1f(NormalShader.get(), NormalShader.uniformLoc("normalTex"), 2);

	//glProgramUniform4f(lambertShader.get(), lambertShader.uniformLoc("color"), 1.f, 1.f, 1.f, 1.f);
	
	//set up Shadow map shader uniforms
	glProgramUniform1f(shadowCubeMapShader.get(), shadowCubeMapShader.uniformLoc("nearPlane"), shadowMapNear);
	glProgramUniform1f(shadowCubeMapShader.get(), shadowCubeMapShader.uniformLoc("farPlane"), shadowMapFar);
	glProgramUniform3f(shadowCubeMapShader.get(), shadowCubeMapShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());

	glProgramUniform4f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("color"), 0.27f, 0.33f, 0.34f, 1.f);
	glProgramUniform1f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("nearPlane"), shadowMapNear);
	glProgramUniform1f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("farPlane"), shadowMapFar);
	glProgramUniform1i(shadowMappedShader.get(), shadowMappedShader.uniformLoc("shadowMap"), 1);
	glProgramUniform1f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("lightRadius"), lightWidth);
	glProgramUniform1i(shadowMappedShader.get(), shadowMappedShader.uniformLoc("sampleRadius"), sampleRadius);
	glProgramUniform1f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("bias"), shadowMapBias);
	glProgramUniform3f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());
	glProgramUniform3fv(shadowMappedShader.get(), shadowMappedShader.uniformLoc("worldLightDir"), 1, worldLightDir.data());
	glProgramUniform1f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("worldLightInt"), worldLightIntensity);

	//set up HDR shader uniforms
	glProgramUniform1i(HDRShader.get(), HDRShader.uniformLoc("HDRBuffer"), 0); /*! set HDR shader buffer to 0 (should be 0 by default)*/
	glProgramUniform1i(HDRShader.get(), HDRShader.uniformLoc("depthTexture"), 2);

	//set up water shader uniforms
	glProgramUniform1f(waterShader.get(), waterShader.uniformLoc("reflectionTexture"), 0);
	glProgramUniform1f(waterShader.get(), waterShader.uniformLoc("clipDist"), 1);
	glProgramUniform1f(waterShader.get(), waterShader.uniformLoc("clipDir"), -1);

	//set up Depth of feild shader uniforms
	glProgramUniform1f(DOFShader.get(), DOFShader.uniformLoc("nearPlane"), shadowMapNear);
	glProgramUniform1f(DOFShader.get(), DOFShader.uniformLoc("farPlane"), shadowMapFar);

	//Create particles (work in progress)
	GLuint ringVao;
	glGenVertexArrays(1, &ringVao);
	glhelper::ShaderStorageBuffer particleBuffer(nRParticles * 4 * sizeof(float)),
		velocityBuffer(nRParticles * 4 * sizeof(float));

	// Initialise ring particle positions and velocities		
	{
		std::vector<Eigen::Vector4f> particlePositions(nRParticles), particleVelocities(nRParticles);
		std::default_random_engine eng;
		std::uniform_real_distribution<> radDist(ringMinRadius, ringMaxRadius);

		for (size_t i = 0; i < nRParticles; ++i) {
			//float angle = angleDist(eng);
			float radius = radDist(eng);

			//particlePositions[i] = radius * Eigen::Vector4f(sinf(angle), 1.f, cosf(angle), 1.0f);
			particlePositions[i].x() = radDist(eng) - 50;
			particlePositions[i].y() = radDist(eng);
			particlePositions[i].z() = radDist(eng) - 50;

			particleVelocities[i] = particleInitialVelocity * Eigen::Vector4f(0.f, 0.f, 0.f, 0.f);
			Eigen::Vector3f vel = -particlePositions[i].block<3, 1>(0, 0).normalized().cross(Eigen::Vector3f(0.f, 0.f, 0.f)) * particleInitialVelocity;
			particleVelocities[i].block<3, 1>(0, 0) = vel;
		}

		particleBuffer.update(particlePositions);
		velocityBuffer.update(particleVelocities);
	}


	glBindVertexArray(ringVao);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//set up particle uniforms
	glProgramUniform1f(billboardParticleShader.get(), billboardParticleShader.uniformLoc("particleSize"), rainParticleSize);
	glProgramUniform3f(billboardParticleShader.get(), billboardParticleShader.uniformLoc("particleColor"), 0.4f, 0.48f, 0.59f);

	glProgramUniform1f(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("gravitationalConstant"), gravitationalConstant);
	glProgramUniform1f(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("timeStep"), 1.f / 33.3f);
	glProgramUniform1f(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("particleMass"), particleMass);


	//Model class re-purposed to be a model loader 
	Model* modelLoader = new Model();

	ReloadConfig();
	


#pragma region Meshes
	//Test Mesh (spot the cow)
		glhelper::Mesh testMesh;
		testMesh.meshName = "TestMesh";

		Eigen::Matrix4f bunnyModelToWorld = Eigen::Matrix4f::Identity();
		bunnyModelToWorld(0, 0) = 0.3f;
		bunnyModelToWorld(1, 1) = 0.3f;
		bunnyModelToWorld(2, 2) = 0.3f;
		bunnyModelToWorld = makeTranslationMatrix(Eigen::Vector3f(6.f, 0.5f, 0)) * makeRotationMatrix(0, 0, 0) * bunnyModelToWorld;
		loadSpotMesh(&testMesh);
		//modelLoader->loadFromFile(/*"../models/stanford_bunny/scene.gltf"*/"../models/spot/spot_triangulated.obj", &testMesh);
		testMesh.loadTexture(/*"../models/stanford_bunny/textures/Bunny_baseColor.png"*/"../models/spot/spot_texture.png");

		testMesh.modelToWorld(bunnyModelToWorld);
		testMesh.shaderProgram(&NormalShader);

		GLuint spotNormalMap = createTexture("../models/spot/normalmap.png");

		GLuint spotTexture = createTexture("../models/spot/spot_texture.png");


		//Sphere mesh used to show light position for debugging 
		glhelper::Mesh sphereMesh;
		sphereMesh.meshName = "Sphere";
		sphereMesh.modelToWorld(makeTranslationMatrix(lampLight->GetPos()));
		modelLoader->loadFromFile("../models/sphere.obj", &sphereMesh);
		sphereMesh.shaderProgram(&lambertShader);
		sphereMesh.setCastsShadow(false);

		//Lighthouse Mesh
		glhelper::Mesh lightHouse;
		lightHouse.meshName = "LightHouse";
		Eigen::Matrix4f lighthouseModelToWorld = Eigen::Matrix4f::Identity();
		lighthouseModelToWorld = makeIdentityMatrix(1);
		lighthouseModelToWorld = makeTranslationMatrix(Eigen::Vector3f(-0.1f, 0.f, 0.f)) * makeRotationMatrix(0, 16, 0) * lighthouseModelToWorld;
		modelLoader->loadFromFile("../models/lighthouse2/source/Lighthouse-scaled.fbx", &lightHouse);
		lightHouse.loadTexture("../models/lighthouse2/textures/Base_color.png");
		GLuint lightHouseNormal = createTexture("../models/lighthouse2/textures/Normal.png");
		lightHouse.modelToWorld(lighthouseModelToWorld);
		lightHouse.shaderProgram(&NormalShader);
		lightHouse.setCastsShadow(false);

		//Rock Mesh
		glhelper::Mesh rock;
		rock.meshName = "Rock";
		Eigen::Matrix4f rockModelToWorld = Eigen::Matrix4f::Identity();
		rockModelToWorld = makeIdentityMatrix(1);
		rockModelToWorld = makeTranslationMatrix(Eigen::Vector3f(-4.7, 0.f, -2.1)) * makeRotationMatrix(0, -180, 0) * rockModelToWorld;
		modelLoader->loadFromFile("../models/obj-nat-rock/source/nat-rock-scaled2.obj", &rock);
		rock.loadTexture("../models/obj-nat-rock/textures/nat-rock-diff.jpeg");
		GLuint rockNormal = createTexture("../models/obj-nat-rock/textures/nat-rock-norm.jpeg");
		rock.shaderProgram(&NormalShader);
		rock.modelToWorld(rockModelToWorld);
		//Second rock mesh
		glhelper::Mesh rock2;
		rock2.meshName = "Rock 2";
		Eigen::Matrix4f rock2ModelToWorld = makeIdentityMatrix(1); 
		modelLoader->loadFromFile("../models/obj-nat-rock/source/nat-rock-Scaled2.obj", &rock2);
		rock2.loadTexture("../models/obj-nat-rock/textures/nat-rock-diff.jpeg");
		rock2.shaderProgram(&blinnPhongShader);
		rock2.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(9, 0.f, -4.1)) * makeRotationMatrix(0,-180,0) * makeScaleMatrix(1) * rock2ModelToWorld);

		//River Rock Mesh
		glhelper::Mesh riverRock;
		riverRock.meshName = "river rock";
		Eigen::Matrix4f riverRockModelToWorld = Eigen::Matrix4f::Identity();
		riverRockModelToWorld = makeIdentityMatrix(0.01f);
		riverRockModelToWorld = makeTranslationMatrix(Eigen::Vector3f(-5, -0.5f, 0)) * riverRockModelToWorld;

		riverRock.loadTexture("../models/river-rock/textures/RiverRock_BaseColor.png");
		modelLoader->loadFromFile("../models/river-rock/source/River_Rock.fbx", &riverRock);
		riverRock.shaderProgram(&lambertShader);
		riverRock.modelToWorld(riverRockModelToWorld);

		glhelper::Mesh groundPlane;
		groundPlane.meshName = "Ground";
		Eigen::Matrix4f groundModelToWorld = Eigen::Matrix4f::Identity();
		groundModelToWorld = makeTranslationMatrix(Eigen::Vector3f(0, 0, 0.f)) * groundModelToWorld;
		modelLoader->loadFromFile("../models/groundPlane.obj", &groundPlane);
		groundPlane.loadTexture("../models/Forest-Ground_01.png");
		groundPlane.shaderProgram(&shadowMappedShader);
		groundPlane.modelToWorld(makeScaleMatrix(2) * groundModelToWorld);

		glhelper::Mesh waterPlane;
		waterPlane.meshName = "Water";
		modelLoader->loadFromFile("../models/groundPlane.obj", &waterPlane);
		waterPlane.shaderProgram(&waterShader);
		Eigen::Matrix4f waterModelToWorld = Eigen::Matrix4f::Identity();
		waterPlane.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(0, 0.1f, 20.f)) * waterModelToWorld);
		

		glhelper::Mesh Tree1;
		Tree1.meshName = "tree1";
		modelLoader->loadFromFile("../models/tree/Tree-scaled.obj", &Tree1);
		Tree1.loadTexture("../models/tree/textures/Ivy_branch_Variation_1_Diffuse-Ivy_branch_Variation_1_Opacit.png");
		Tree1.shaderProgram(&blinnPhongShader);
		Eigen::Matrix4f treeModelToWorld = Eigen::Matrix4f::Identity();
		Tree1.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(4, 0, 4.f)) * makeScaleMatrix(1) * treeModelToWorld);

		glhelper::Mesh pinecone;
		pinecone.meshName = "Pinecone";
		modelLoader->loadFromFile("../models/pinecone/pinecone.fbx", & pinecone);
		pinecone.loadTexture("../models/pinecone/pinecone_1001_BaseColor.jpg");
		pinecone.shaderProgram(&blinnPhongShader);
		Eigen::Matrix4f pineconeMTW = Eigen::Matrix4f::Identity();
		pinecone.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(4.2f, 0, 4.2f)) * makeScaleMatrix(30.f) * pineconeMTW);

#pragma endregion

		//BULLET physics
		
			// This sets up the physics world for your simulation.
			// I used unique_ptr here for convenience but it would probably be neater
			// to wrap up the creation and destruction of the physics world into a class.
			std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig =
				std::make_unique<btDefaultCollisionConfiguration>();
			std::unique_ptr<btCollisionDispatcher> dispatcher =
				std::make_unique<btCollisionDispatcher>(collisionConfig.get());
			std::unique_ptr<btBroadphaseInterface> overlappingPairCache =
				std::make_unique<btDbvtBroadphase>();
			std::unique_ptr<btSequentialImpulseConstraintSolver> solver =
				std::make_unique<btSequentialImpulseConstraintSolver>();
			std::unique_ptr<btDiscreteDynamicsWorld> world =
				std::make_unique<btDiscreteDynamicsWorld>(
					dispatcher.get(), overlappingPairCache.get(), solver.get(), collisionConfig.get());

			// You may want to set appropriate gravity here.
			world->setGravity(btVector3(0, -1, 0));
			// When you create your shapes (box and sphere) add them to this array. 
			// You should then delete them at the end of the program.
			btAlignedObjectArray<btCollisionShape*> collisionShapes;
			{
				// Make a rigidbody for the floor
				// Make a box collision shape, set its transform, mass, inertia and restitution
				// then make the rigidbody with these properties and add it to the world.
				btBoxShape* box;
				box = new btBoxShape(btVector3(20, 0.1f, 20));
				btRigidBody* floor;
				btRigidBody::btRigidBodyConstructionInfo floorInfo{ 0, 0, box };
				floor = new btRigidBody(floorInfo);
				floor->setRestitution(1.0f);
				floor->setCollisionShape(box);
				floor->setWorldTransform(btTransform(btQuaternion(0, 0, 0), btVector3(0.0f, 0.f, 0.0f)));
				world->addCollisionObject(floor);
			}
			btRigidBody* ball;

			{
				// Make a rigidbody for the ball
				// Make a sphere collision shape, set its transform, mass, inertia and restitution
				// then make the rigidbody with these properties and add it to the world.
				// I recommend setting body->setActivationState(DISABLE_DEACTIVATION)
				// By default, the sphere will be dectivated if it stops moving and you'll need to call
				// body->activate(); again for impulses and forces to have any effect.
				// This is more efficient, but annoying for debugging!
				btSphereShape* sphere;
				sphere = new btSphereShape(btScalar(1));
				btTransform ballTr;
				ballTr.setOrigin(btVector3(0.0f, 5.f, 0));
				//sphere->calculateLocalInertia(1, btVector3(0, 0, 0));
				btDefaultMotionState* ballMS = new btDefaultMotionState(ballTr);

				btRigidBody::btRigidBodyConstructionInfo ballInfo{ 1, ballMS, sphere };
				ball = new btRigidBody(ballInfo);
				ball->setRestitution(1.f);
				ball->setCollisionShape(sphere);
				ball->setWorldTransform(btTransform(btQuaternion(0, 0, 0), btVector3(4.1f, 5.0f, 4.0f)));
				ball->setActivationState(DISABLE_DEACTIVATION);
				world->addRigidBody(ball);
			}

		
		//Colour buffer

		GLuint HDRFrameBuffer;
		glGenFramebuffers(1, &HDRFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);

		GLuint colourBuffer;
		glGenTextures(1, &colourBuffer);
		glBindTexture(GL_TEXTURE_2D, colourBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		//second colour buffer for use in depth of field 
		//GLuint colourBuffer2;
		//glGenTextures(1, &colourBuffer2);
		//glBindTexture(GL_TEXTURE_2D, colourBuffer2);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourBuffer, 0);
		
		
		//glStencilFunc(GL_ALWAYS, 1, depthTexture);
		
		//attach depth buffer
		GLuint renderBuffer;
		glGenRenderbuffers(1, &renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

		//Colour buffer error check 
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Colour buffer not created correctly: " << status << std::endl;
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Depth frame buffer used for depth of field calculations
		GLuint dframeBuffer;
		glGenFramebuffers(1, &dframeBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, dframeBuffer);
		GLuint depthTexture;
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		//Create cubemap for shadows
		GLuint cubeMapTexture;
		glGenTextures(1, &cubeMapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTextureParameteri(cubeMapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		Eigen::Matrix4f cubemapPerspective;

		cubemapPerspective = perspective(M_PI_2, 1, shadowMapNear, shadowMapFar);

		// Here are the rotations for each face of the cubemap (please do use them!)
		 std::array<Eigen::Matrix4f, 6> cubemapRotations{
			angleAxisMat4(float(M_PI_2), Eigen::Vector3f(0,1,0)),//POSITIVE_X - rotate right 90 degrees
			angleAxisMat4(float(-M_PI_2), Eigen::Vector3f(0,1,0)),//NEGATIVE_X - rotate left 90 degrees
			angleAxisMat4(float(-M_PI_2), Eigen::Vector3f(1,0,0)) * angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),//POSITIVE_Y - rotate up 90 degrees
			angleAxisMat4(float(M_PI_2), Eigen::Vector3f(1,0,0)) * angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),//NEGATIVE_Y - rotate down 90 degrees
			angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),     //POSITIVE_Z - rotate right 180 degrees
			Eigen::Matrix4f::Identity()                           //NEGATIVE_Z
		};


		//TODO remove scene and replace with World class, currently only used for shadows
		std::vector<glhelper::Renderable*> scene{ /*&testMesh,*//* &lightHouse,*/ &rock, &rock2, &groundPlane};

		//set up world scene
		Worldscene = new RGLib::World;
		//Worldscene->AddToWorld(testMesh);
		Worldscene->SetShadowMapShaders(shadowMappedShader, shadowCubeMapShader);

		Worldscene->AddWorldLight(*lampLight);
		//Worldscene->AddToWorld(groundPlane);
		//Worldscene->AddToWorld(sphereMesh);
		//Worldscene->AddToWorld(lightHouse);
		//Worldscene->AddToWorld(rock);
		//Worldscene->AddToWorld(Tree1);
		//Worldscene->AddToWorld(pinecone);
		Worldscene->AddWorldObject(Tree1);
		Worldscene->AddWorldObject(pinecone);
		Worldscene->ground = &groundPlane;
		Worldscene->AddWorldObject(testMesh, spotNormalMap);
		Worldscene->AddWorldObject(lightHouse, lightHouseNormal);
		Worldscene->AddWorldObject(rock, rockNormal);
		Worldscene->AddWorldObject(rock2, rockNormal);
		//RGLib::WorldObject* lightHouse = new RGLib::WorldObject(lightHouse, Worldscene);

		for (auto& model : models) {
			glhelper::Mesh& mesh = meshes.at(model["mesh"]);
			glhelper::ShaderProgram& shader = shaders.at(model["shader"]);
			glhelper::Texture& texture = textures.at(model["texture"]);
			texture.bindToImageUnit(0);
			glProgramUniform1i(shader.get(), shader.uniformLoc("tex"), 0);
			mesh.shaderProgram(&shader);
			//mesh.meshTex = std::make_unique< glhelper::Texture>(GL_TEXTURE_2D, GL_RGB8, texture.height(), texture.width(),
			//	0, GL_RGB, GL_UNSIGNED_BYTE, texture.getData(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

			Eigen::Vector3f position(model["position"][0], model["position"][1], model["position"][2]);
			mesh.modelToWorld(makeTranslationMatrix(position));
			mesh.setTexture(texture);
			//Worldscene->AddToWorld(mesh);
		}

		Worldscene->models = &models;
		//Worldscene->meshes = &meshes;
		//Worldscene->shaders = &shaders;

		Worldscene->CreateQueries();

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		bool lightRotating = false;
		bool running = true;
		float lightDir = 0;
		float lightDir2 = 0;
		Eigen::Matrix4f lightMat = Eigen::Matrix4f::Identity();
		lightMat = makeRotationMatrix(0, 0, 0);
		glm::vec3 glmLight;
		glmLight = glm::vec3(90, 0, 0);

		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		RGLib::FrameBuffer* reflectionBuffer = new RGLib::FrameBuffer(1280, 720);
		reflectionBuffer->init();
		//unsigned int textureframe = frameBuffer2->getTextureLocation();

		Eigen::Vector3f spotLightDir(-1.0f, 0.f, 0.f);
		Eigen::AngleAxisf rotation;
		Eigen::Vector3f rotDir;
		bool hdrEnable = false;
	
		while (running)
		{
			Uint64 frameStartTime = SDL_GetTicks64();

			world->stepSimulation((desiredFrametime / 1000.0f), 10);


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
#pragma region KeyInput
				if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_DOWN) {
						if (event.key.keysym.mod & KMOD_SHIFT) {
							lampLight->setIntensity(lampLight->Intensity() - 0.1f);
							if (lampLight->Intensity() < 0.f) lampLight->setIntensity(0);
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightIntensity"), lampLight->Intensity());
						}
						else {
							lampLight->setIntensity(lampLight->Intensity() - 10.f);
							if (lampLight->Intensity() < 0.f) lampLight->setIntensity(0);
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightIntensity"), lampLight->Intensity());
						}
					}
					if (event.key.keysym.sym == SDLK_UP) {
						if (event.key.keysym.mod & KMOD_SHIFT) {
							lampLight->setIntensity(lampLight->Intensity() + 0.1f);
							if (lampLight->Intensity() < 0.f) lampLight->setIntensity(0);
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightIntensity"), lampLight->Intensity());
						}
						else {
							lampLight->setIntensity(lampLight->Intensity() + 10.f);
							if (lampLight->Intensity() < 0.f) lampLight->setIntensity(0);
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightIntensity"), lampLight->Intensity());
						}
					}
					if (event.key.keysym.sym == SDLK_RIGHT) {
						if (event.key.keysym.mod & KMOD_SHIFT) {
							fallOffExponent += 0.01f;
							if (fallOffExponent < 0.f) fallOffExponent = 0.f;
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("falloffExponent"), fallOffExponent);
						}
						else {
							fallOffExponent += 10.0f;
							if (fallOffExponent < 0.f) fallOffExponent = 0.f;
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("falloffExponent"), fallOffExponent);
						}
					}
					if (event.key.keysym.sym == SDLK_LEFT) {
						if (event.key.keysym.mod & KMOD_SHIFT) {
							fallOffExponent -= 0.01f;
							if (fallOffExponent < 0.f) fallOffExponent = 0.f;
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("falloffExponent"), fallOffExponent);
						}
						else {
							fallOffExponent -= 10.0f;
							if (fallOffExponent < 0.f) fallOffExponent = 0.f;
							glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("falloffExponent"), fallOffExponent);
						}
					}
					if (event.key.keysym.sym == SDLK_p)
					{
						ReloadConfig();
					}
					if (event.key.keysym.sym == SDLK_o)
					{
						if (hdrEnable)
							hdrEnable = false;
						else
							hdrEnable = true;
						glProgramUniform1i(HDRShader.get(), HDRShader.uniformLoc("enabled"), hdrEnable);

						std::cout << "HDR ENABLED = " << hdrEnable << std::endl;
					}
					
				}
#pragma endregion
			}

			if (lightRotating) {
				theta += 0.01f;
				if (theta > 2 * 3.14159f) theta = 0.f;
				lampLight->GetPos() << 5.f * sinf(theta), 5.f, 5.f * cosf(theta);
				sphereMesh.modelToWorld(makeTranslationMatrix(lampLight->GetPos()));
				glProgramUniform3f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());
				glProgramUniform3f(NormalShader.get(), NormalShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());

			}

			//Rotate light code (does not work properly with the shader)
	/*		if (lightDir < 2 * M_PI)
			{
				lightDir += 0.1f;
				lightDir2 -= 0.1f;
			}
			else
			{
				lightDir = -6.f;
				lightDir2 = -6.f;
			}*/
			lightDir += 0.01f;
			if (lightDir > 2.0f * M_PI)
			{
				lightDir = 0;
			}

			rotation = Eigen::AngleAxisf(-lightDir, Eigen::Vector3f::UnitY());
			rotDir = rotation * spotLightDir;

			lightMat = makeRotationMatrix(90, 0, 0);
			glProgramUniform3f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("spotLightDir"), rotDir.x(), rotDir.y(), rotDir.z());
			glProgramUniform3f(NormalShader.get(), NormalShader.uniformLoc("spotLightDir"), rotDir.x(), rotDir.y(), rotDir.z());
			glProgramUniform3f(shadowMappedShader.get(), shadowMappedShader.uniformLoc("spotLightDir"), rotDir.x(), rotDir.y(), rotDir.z());

			//Render reflections
			reflectionBuffer->bind();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CLIP_DISTANCE0);
			Worldscene->RenderWorldObjects();
			glDisable(GL_CLIP_DISTANCE0);
			reflectionBuffer->unbind();
			//Clear buffers before rendering shadows
			glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			Eigen::Matrix4f flipMatrix;
			flipMatrix <<
				-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0, 0, 1, 0,
				0, 0, 0, 1;


			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glDisable(GL_CULL_FACE);
			glViewport(0, 0, shadowMapSize, shadowMapSize);


			for (int i = 0; i < 6; ++i) {

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture, 0);
				glClear(GL_DEPTH_BUFFER_BIT);

				Eigen::Matrix4f clipMatrix;

				clipMatrix = flipMatrix * cubemapPerspective * cubemapRotations[i] * makeTranslationMatrix(-lampLight->GetPos());


				glProgramUniformMatrix4fv(shadowCubeMapShader.get(), shadowCubeMapShader.uniformLoc("shadowWorldToClip"), 1, false, clipMatrix.data());

				for (glhelper::Renderable* mesh : scene)
				{
					if (mesh->castsShadow())
						mesh->render(shadowCubeMapShader);

				}

			}
			//Worldscene->CreateShadowMaps();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			glViewport(0, 0, windowWidth, windowHeight);

		/*	for (glhelper::Renderable* mesh : scene) {

				mesh->render();

			}*/
			//glDisable(GL_CULL_FACE);
			//Worldscene->RenderShadowMaps();


			glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);
			glEnable(GL_DEPTH_TEST);
			glClearDepth(1.0);
			glEnable(GL_STENCIL_TEST);
			//Render scene
			glActiveTexture(GL_TEXTURE0 + 0);
			//Stencil functions from https://open.gl/depthstencils
			//glBindTexture(GL_TEXTURE_2D, reflectionBuffer->getTextureLocation());
			glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilMask(0xFF); // Write to stencil buffer
			glDepthMask(GL_FALSE);
			glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
			waterPlane.render();
			glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
			glStencilMask(0x00); // Don't write anything to stencil buffer
			glDepthMask(GL_TRUE);
			glProgramUniform3f(NormalShader.get(), NormalShader.uniformLoc("colourOverride"), 0.0f, 0.0f, 0.1f);
			Worldscene->RenderReflectedObjects();
			glProgramUniform3f(NormalShader.get(), NormalShader.uniformLoc("colourOverride"), 0.0f, 0.0f, 0.0f);

			glDisable(GL_STENCIL_TEST);
			//end source
			//glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
			glActiveTexture(GL_TEXTURE0 + 0);
			groundPlane.meshTex->bindToImageUnit(0);
			groundPlane.render();
			//Worldscene->RenderShadowMaps();

			glActiveTexture(GL_TEXTURE0 + 0);

			btTransform ballTran;
			ball->getMotionState()->getWorldTransform(ballTran);
			btVector3 ballPos = ballTran.getOrigin();
			//std::cout << "ball pos: " << ballPos.y() << std::endl;
			pinecone.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(ballPos.x(), ballPos.y(), ballPos.z()) ) * makeScaleMatrix(0.2f));

			Worldscene->RenderWorldObjects();

			//render Rain particles from research paper (Work in progress)
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer.get());

			//glhelper::BufferObject velocityBuffer(nParticles, GL_SHADER_STORAGE_BUFFER);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer.get());

			glUseProgram(RainPhysicsShader.get());
			glDispatchCompute(nRParticles, 1, 1);

			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
			glBindVertexArray(ringVao);
			billboardParticleShader.use();
			glDrawArrays(GL_POINTS, 0, nRParticles);
			billboardParticleShader.unuse();
			glDepthMask(GL_TRUE);

			//std::cout << "camPOs " << viewer.position().x() << viewer.position().y() << viewer.position().z() << std::endl;
			glProgramUniform3f(DOFShader.get(), DOFShader.uniformLoc("camPosWorld"), viewer.position().x(), viewer.position().y(), viewer.position().z());
			//glProgramUniform3f(DOFShader.get(), DOFShader.uniformLoc("camPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());
			Eigen::Matrix4f clipMatrix;
			clipMatrix = flipMatrix * cubemapPerspective *  makeTranslationMatrix(-Eigen::Vector3f(cam->getPosX(), cam->getPosY(), cam->getPosZ()));


			glProgramUniformMatrix4fv(DOFShader.get(), DOFShader.uniformLoc("shadowWorldToClip"), 1, false, clipMatrix.data());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, dframeBuffer);
			glDrawBuffer(GL_NONE);
			glDisable(GL_CULL_FACE);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			Worldscene->RenderWorldObjects(DOFShader);


			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);

			//glCopyImageSubData(colourBuffer, GL_TEXTURE_2D, 0, 0, 0, 0, colourBuffer2, GL_TEXTURE_2D, 0, 0, 0, 0, windowWidth, windowHeight, 1);
			////RENDER QUAD TO SCREEN
			HDRShader.use();
			glDisable(GL_DEPTH_TEST);
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, colourBuffer);
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			renderQuad();

			Worldscene->RenderGUI();
			////glhelper::Mesh* tM = &testMesh;

			//glProgramUniform1i(NormalShader.get(), NormalShader.uniformLoc("albedoTex"), 0);
			//glProgramUniform1i(NormalShader.get(), NormalShader.uniformLoc("normalTex"), 2);




			

			SDL_GL_SwapWindow(window);

			//TODO replace with v-sync code with option to turn off
			//Used to limit framerate
			/*Uint64 elapsedFrameTime = SDL_GetTicks64() - frameStartTime;
			if (elapsedFrameTime < desiredFrametime) {
				SDL_Delay(desiredFrametime - elapsedFrameTime);
			}*/
		};

		//Clean up
		Clean();
		glDeleteBuffers(1, &HDRFrameBuffer);
		glDeleteBuffers(1, &dframeBuffer);
		glDeleteBuffers(1, &colourBuffer);
		delete(reflectionBuffer);
		Worldscene->Clean();

		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
	}

void Clean()
{
}