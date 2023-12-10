#include "RGLib/ShadowMap.hpp"

RGLib::ShadowMap::ShadowMap(int mapSize, RGLib::Light* light)
{
	//Set up shader
	shadowCubeMapShader = new glhelper::ShaderProgram({ "..\\shaders\\ShadowCubeMap.vert", "..\\shaders\\ShadowCubeMap.frag" });
	shadowMappedShader = new glhelper::ShaderProgram({ "..\\shaders\\ShadowMapped.vert", "..\\shaders\\ShadowMapped.frag" });

	glProgramUniform1f(shadowCubeMapShader->get(), shadowCubeMapShader->uniformLoc("nearPlane"), shadowMapNear);
	glProgramUniform1f(shadowCubeMapShader->get(), shadowCubeMapShader->uniformLoc("farPlane"), shadowMapFar);
	glProgramUniform3f(shadowCubeMapShader->get(), shadowCubeMapShader->uniformLoc("lightPosWorld"), light->getX(), light->getY(), light->getZ());

	glProgramUniform4f(shadowMappedShader->get(), shadowMappedShader->uniformLoc("color"), 0.27f, 0.33f, 0.34f, 1.f);
	glProgramUniform1f(shadowMappedShader->get(), shadowMappedShader->uniformLoc("nearPlane"), shadowMapNear);
	glProgramUniform1f(shadowMappedShader->get(), shadowMappedShader->uniformLoc("farPlane"), shadowMapFar);
	glProgramUniform1i(shadowMappedShader->get(), shadowMappedShader->uniformLoc("shadowMap"), 1);
	glProgramUniform1f(shadowMappedShader->get(), shadowMappedShader->uniformLoc("lightRadius"), light->lightWidth);
	glProgramUniform1i(shadowMappedShader->get(), shadowMappedShader->uniformLoc("sampleRadius"), light->lightRadius);
	glProgramUniform1f(shadowMappedShader->get(), shadowMappedShader->uniformLoc("bias"), shadowMapBias);
	glProgramUniform3f(shadowMappedShader->get(), shadowMappedShader->uniformLoc("lightPosWorld"), light->getX(), light->getY(), light->getZ());

	//Set up cubemap
	glGenTextures(1, &cubeMapTexture);

	glTextureParameteri(cubeMapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	Eigen::Matrix4f cubemapPerspective;

	cubemapPerspective = perspective(M_PI_2, 1, shadowMapNear, shadowMapFar);

	//Set up frameBuffer
	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//set up flip matrix
	flipMatrix <<
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0, 0, 1, 0,
		0, 0, 0, 1;

}

std::array<Eigen::Matrix4f, 6> cubemapRotations{
   angleAxisMat4(float(M_PI_2), Eigen::Vector3f(0,1,0)),//POSITIVE_X - rotate right 90 degrees
   angleAxisMat4(float(-M_PI_2), Eigen::Vector3f(0,1,0)),//NEGATIVE_X - rotate left 90 degrees
   angleAxisMat4(float(-M_PI_2), Eigen::Vector3f(1,0,0)) * angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),//POSITIVE_Y - rotate up 90 degrees
   angleAxisMat4(float(M_PI_2), Eigen::Vector3f(1,0,0)) * angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),//NEGATIVE_Y - rotate down 90 degrees
   angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),     //POSITIVE_Z - rotate right 180 degrees
   Eigen::Matrix4f::Identity()                           //NEGATIVE_Z
};



void RGLib::ShadowMap::RenderShadowMap(std::vector<glhelper::Mesh*> shadowCasters, std::vector<glhelper::Mesh*> shadowRecivers, RGLib::Light* light)
{

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, shadowMapSize, shadowMapSize);


	for (int i = 0; i < 6; ++i) {

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		Eigen::Matrix4f clipMatrix;

		clipMatrix = flipMatrix * cubemapPerspective * cubemapRotations[i] * makeTranslationMatrix(-light->GetPos());


		glProgramUniformMatrix4fv(shadowCubeMapShader->get(), shadowCubeMapShader->uniformLoc("shadowWorldToClip"), 1, false, clipMatrix.data());

		for (glhelper::Renderable* mesh : shadowCasters)
		{
			if (mesh->castsShadow())
				mesh->render(*shadowCubeMapShader);

		}

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	for (glhelper::Mesh* mesh : shadowRecivers)
	{
		mesh->render(*shadowMappedShader);
	}

}
