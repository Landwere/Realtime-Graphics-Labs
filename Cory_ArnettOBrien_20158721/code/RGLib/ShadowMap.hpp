#pragma once
#include <GL/glew.h>
#include <Eigen/Dense>
#include "RGLib/ShaderProgram.hpp"
#include "RGLib/Texture.hpp"
#include "RGLib/Mesh.hpp"
#include "RGLib/Light.hpp"
#include "RGLib/Matrices.hpp"
#include "Constants.hpp"
#define SDL_MAIN_HANDLED

namespace RGLib
{
	class ShadowMap
	{
	public:
		ShadowMap(int mapSize, RGLib::Light* light);


		void RenderShadowMap(std::vector<glhelper::Mesh*> shadowCasters, std::vector<glhelper::Mesh*> shadowRecivers, RGLib::Light* light);

		void CreateCubeMap(std::vector<glhelper::Mesh*> shadowCasters, RGLib::Light* light);

	private:
		Eigen::Matrix4f flipMatrix;
		int shadowMapSize;
		const float shadowMapNear = 1.f, shadowMapFar = 1000.f;
		float shadowMapBias = 1.0f;
		GLuint cubeMapTexture;
		Eigen::Matrix4f cubemapPerspective;
		glhelper::ShaderProgram* shadowMappedShader;
		glhelper::ShaderProgram* shadowCubeMapShader;
		GLuint frameBuffer;
	};





}