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
float shadowMapBias = 1.0f;
const int shadowMapSize = 1024;

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
float particleMass = 0.1f;
float gravitationalConstant = -0.1f;
float ringParticleSize = 0.03f;
const int MAX_N_MASSES = 10;


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
//end source
int main()
{
	RGLib::Light* lampLight = new RGLib::Light(100, Eigen::Vector3f(-0.1f, 7.f, 0.f));

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

	//glhelper::RotateViewer viewer(windowWidth, windowHeight);
	glhelper::FlyViewer viewer(windowWidth, windowHeight);

	//Shader
	glhelper::ShaderProgram lambertShader({ "..\\shaders\\Lambert.vert", "..\\shaders\\Lambert.frag" });
	glhelper::ShaderProgram blinnPhongShader({ "..\\shaders\\BlinnPhong.vert", "..\\shaders\\BlinnPhong.frag" });
	glhelper::ShaderProgram shadowCubeMapShader({ "..\\shaders\\ShadowCubeMap.vert", "..\\shaders\\ShadowCubeMap.frag" });
	glhelper::ShaderProgram shadowMappedShader({ "..\\shaders\\ShadowMapped.vert", "..\\shaders\\ShadowMapped.frag" });
	glhelper::ShaderProgram billboardParticleShader({ "../shaders/BillboardParticle.vert", "../shaders/BillboardParticle.geom", "../shaders/BillboardParticle.frag" });
	glhelper::ShaderProgram RainPhysicsShader({ "../shaders/RainParticle.comp" });
	glhelper::ShaderProgram NormalShader({ "..\\shaders\\NormalShader.vert", "..\\shaders\\NormalShader.frag" });
	glhelper::ShaderProgram HDRShader({ "..\\shaders\\HDRShader.vert", "..\\shaders\\HDRShader.frag" });

	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("specularExponent"), specularExponent);
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("specularIntensity"), specularIntensity);
	glProgramUniform3f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightPosWorld"), lampLight->getX(), lampLight->getY(), lampLight->getZ());
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("lightIntensity"), lampLight->Intensity());
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("falloffExponent"), fallOffExponent);
	glProgramUniform3fv(blinnPhongShader.get(), blinnPhongShader.uniformLoc("worldLightDir"), 1, worldLightDir.data());
	glProgramUniform1f(blinnPhongShader.get(), blinnPhongShader.uniformLoc("worldLightInt"), worldLightIntensity);

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

	glProgramUniform1i(HDRShader.get(), HDRShader.uniformLoc("HDRBuffer"), 0);

	GLuint ringVao;
	glGenVertexArrays(1, &ringVao);
	glhelper::ShaderStorageBuffer particleBuffer(nRParticles * 4 * sizeof(float)),
		velocityBuffer(nRParticles * 4 * sizeof(float));

	// Initialise ring particle positions and velocities		
	{
		std::vector<Eigen::Vector4f> particlePositions(nRParticles), particleVelocities(nRParticles);
		std::default_random_engine eng;
		std::uniform_real_distribution<> radDist(ringMinRadius, ringMaxRadius), angleDist(0.0f, 2.0f * (float)M_PI);

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
	std::array<Eigen::Vector3f, MAX_N_MASSES> massLocations;
	std::array<float, MAX_N_MASSES> masses;
	int nMasses = 1;

	glBindVertexArray(ringVao);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glProgramUniform1f(billboardParticleShader.get(), billboardParticleShader.uniformLoc("particleSize"), ringParticleSize);
	glProgramUniform3f(billboardParticleShader.get(), billboardParticleShader.uniformLoc("particleColor"), 0.4f, 0.48f, 0.59f);

	glProgramUniform3fv(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("massPositions"), MAX_N_MASSES, massLocations[0].data());
	//glProgramUniform1fv(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("masses"), MAX_N_MASSES, &(masses[0]));
	//glProgramUniform1i(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("nMasses"), nMasses);
	glProgramUniform1f(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("gravitationalConstant"), gravitationalConstant);
	glProgramUniform1f(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("timeStep"), 1.f / 33.3f);
	glProgramUniform1f(RainPhysicsShader.get(), RainPhysicsShader.uniformLoc("particleMass"), particleMass);


	//Model class repurposed to be a model loader 
	Model* modelLoader = new Model();

	ReloadConfig();
	


#pragma region Meshes
	//Test Mesh (stanford bunny)
		glhelper::Mesh testMesh;
		testMesh.meshName = "TestMesh";

		Eigen::Matrix4f bunnyModelToWorld = Eigen::Matrix4f::Identity();
		bunnyModelToWorld(0, 0) = 0.2f;
		bunnyModelToWorld(1, 1) = 0.2f;
		bunnyModelToWorld(2, 2) = 0.2f;
		bunnyModelToWorld = makeTranslationMatrix(Eigen::Vector3f(6.f, 0.5f, 0)) * makeRotationMatrix(0, 0, 0) * bunnyModelToWorld;
		loadSpotMesh(&testMesh);
		//modelLoader->loadFromFile(/*"../models/stanford_bunny/scene.gltf"*/"../models/spot/spot_triangulated.obj", &testMesh);
		testMesh.loadTexture(/*"../models/stanford_bunny/textures/Bunny_baseColor.png"*/"../models/spot/spot_texture.png");

		testMesh.modelToWorld(bunnyModelToWorld);
		testMesh.shaderProgram(&NormalShader);

		GLuint spotNormalMap;
		{
			cv::Mat image = cv::imread("../models/spot/normalmap.png");
			cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
			spotNormalMap = createTexture(image);
		}
		GLuint spotTexture;
		{
			cv::Mat image = cv::imread("../models/spot/spot_texture.png");
			cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
			spotTexture = createTexture(image);
		}

		//Sphere mesh
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
		lighthouseModelToWorld = makeTranslationMatrix(Eigen::Vector3f(-0.1f, 0.f, 0.f)) * makeRotationMatrix(16, 0, -90) * lighthouseModelToWorld;

		modelLoader->loadFromFile("../models/lighthouse2/source/Lighthouse.fbx", &lightHouse);
		lightHouse.loadTexture("../models/lighthouse2/textures/Base_color.png");

		lightHouse.modelToWorld(lighthouseModelToWorld);
		lightHouse.shaderProgram(&blinnPhongShader);
		lightHouse.setCastsShadow(false);

		//Rock Mesh
		glhelper::Mesh rock;
		rock.meshName = "Rock";

		Eigen::Matrix4f rockModelToWorld = Eigen::Matrix4f::Identity();
		rockModelToWorld = makeIdentityMatrix(1);
		rockModelToWorld = makeTranslationMatrix(Eigen::Vector3f(-0.7, 0.7f, 2.1)) * makeRotationMatrix(0, 0, -0) * rockModelToWorld;
		modelLoader->loadFromFile("../models/obj-nat-rock/source/nat-rock-scaled.obj", &rock);
		rock.loadTexture("../models/obj-nat-rock/textures/nat-rock-diff.jpeg");
		rock.shaderProgram(&blinnPhongShader);
		rock.modelToWorld(rockModelToWorld);

		glhelper::Mesh rock2;
		Eigen::Matrix4f rock2ModelToWorld = makeTranslationMatrix(Eigen::Vector3f(4.32f, 0.3f, -7.75f));
		modelLoader->loadFromFile("../models/obj-nat-rock/source/nat-rock-scaled.obj", &rock2);
		rock2.loadTexture("../models/obj-nat-rock/textures/nat-rock-diff.jpeg");
		rock2.shaderProgram(&blinnPhongShader);
		rock2.modelToWorld(rock2ModelToWorld);

		//Second Rock Mesh
		glhelper::Mesh riverRock;
		riverRock.meshName = "rock 2";
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
		groundModelToWorld = makeTranslationMatrix(Eigen::Vector3f(0, 0, 6.5f)) * groundModelToWorld;
		modelLoader->loadFromFile("../models/groundPlane.obj", &groundPlane);
		groundPlane.shaderProgram(&shadowMappedShader);
		groundPlane.modelToWorld(makeScaleMatrix(2) * groundModelToWorld);

		glhelper::Mesh waterPlane;
		waterPlane.meshName = "Water";
		modelLoader->loadFromFile("../models/groundPlane.obj", &waterPlane);
		waterPlane.shaderProgram(&blinnPhongShader);
		Eigen::Matrix4f waterModelToWorld = Eigen::Matrix4f::Identity();
		waterPlane.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(0, 0, 40.f)) * waterModelToWorld);


		glhelper::Mesh Tree1;
		Tree1.meshName = "tree1";
		modelLoader->loadFromFile("../models/tree/Tree.obj", &Tree1);
		Tree1.shaderProgram(&blinnPhongShader);
		Eigen::Matrix4f treeModelToWorld = Eigen::Matrix4f::Identity();
		Tree1.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(4, 0, 4.f))* treeModelToWorld);

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
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourBuffer, 0);
		
		GLuint renderBuffer;
		glGenRenderbuffers(1, &renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);

		//attach depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

		//Colour buffer error check 
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Colour buffer not created correctly: " << status << std::endl;
		
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
		Worldscene->AddToWorld(lightHouse);
		Worldscene->AddToWorld(rock);
		Worldscene->AddToWorld(rock2);
		Worldscene->AddToWorld(Tree1);
		Worldscene->AddToWorld(pinecone);
		Worldscene->ground = &groundPlane;
		Worldscene->AddWorldObject(testMesh, spotNormalMap);
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

			//Clear buffers before rendering shadows
			glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);


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
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			glViewport(0, 0, windowWidth, windowHeight);

		/*	for (glhelper::Renderable* mesh : scene) {

				mesh->render();

			}*/
			//glDisable(GL_CULL_FACE);
			//Worldscene->RenderShadowMaps();

			//reflectionBuffer->bind();
			glBindFramebuffer(GL_FRAMEBUFFER, HDRFrameBuffer);
			//glClearColor(0.1f, 0.13f, 0.17f, 1.0f);

		//glDepthFunc(GL_LESS);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
			//Worldscene->RenderWorldObjects();

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
			
			groundPlane.render();
			glActiveTexture(GL_TEXTURE0 + 0);

			btTransform ballTran;
			ball->getMotionState()->getWorldTransform(ballTran);
			btVector3 ballPos = ballTran.getOrigin();
			std::cout << "ball pos: " << ballPos.y() << std::endl;
			pinecone.modelToWorld(makeTranslationMatrix(Eigen::Vector3f(ballPos.x(), ballPos.y(), ballPos.z()) ) * makeScaleMatrix(0.2f));

			//reflectionBuffer->unbind();
			Worldscene->RenderWorldObjects();


			//glDisable(GL_BLEND);
			//glActiveTexture(GL_TEXTURE0 + 0);
			//glBindTexture(GL_TEXTURE_2D, spotTexture);
			//glActiveTexture(GL_TEXTURE0 + 2);
			//glBindTexture(GL_TEXTURE_2D, spotNormalMap);
			//testMesh.render();
			//glBindTexture(GL_TEXTURE_2D, NULL);
			//glActiveTexture(GL_TEXTURE0 + 0);
			//glBindTexture(GL_TEXTURE_2D, NULL);

			//glActiveTexture(GL_TEXTURE0 + 0);
			//glBindTexture(GL_TEXTURE_2D, reflectionBuffer->getTextureLocation());
			//waterPlane.render();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			////RENDER QUAD TO SCREEN
			HDRShader.use();
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, colourBuffer);
			renderQuad();

			//Worldscene->RenderGUI();
			////glhelper::Mesh* tM = &testMesh;

			//glProgramUniform1i(NormalShader.get(), NormalShader.uniformLoc("albedoTex"), 0);
			//glProgramUniform1i(NormalShader.get(), NormalShader.uniformLoc("normalTex"), 2);



			//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer.get());

			////glhelper::BufferObject velocityBuffer(nParticles, GL_SHADER_STORAGE_BUFFER);
			//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer.get());
			//glUseProgram(RainPhysicsShader.get());
			//glDispatchCompute(nRParticles, 1, 1);

			//glEnable(GL_BLEND);
			//glDepthMask(GL_FALSE);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
			//glBindVertexArray(ringVao);
			//billboardParticleShader.use();
			//glDrawArrays(GL_POINTS, 0, nRParticles);
			//billboardParticleShader.unuse();
			//glDepthMask(GL_TRUE);
			//

			SDL_GL_SwapWindow(window);

			//TODO replace with v-sync code with option to turn off
			//Used to limit framerate
			/*Uint64 elapsedFrameTime = SDL_GetTicks64() - frameStartTime;
			if (elapsedFrameTime < desiredFrametime) {
				SDL_Delay(desiredFrametime - elapsedFrameTime);
			}*/
		};

		//Clean up

		return 0;
	}

void Clean()
{

}