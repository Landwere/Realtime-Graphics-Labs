#include "World.hpp"
#include "World.hpp"
#include "World.hpp"
#include "Constants.hpp"
#include <glm/ext/matrix_transform.hpp>





RGLib::World::World()
{
	gltInit();

	dataFile.open("renderData.csv", std::ios_base::app);
	fpsText = gltCreateText();
	frameCountText = gltCreateText();
	frameCount = 0;
	frameSync = 0;
	flipMatrix <<
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0, 0, 1, 0,
		0, 0, 0, 1;


	//Create cubemap for shadows
	glGenTextures(1, &cubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTextureParameteri(cubeMapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	cubemapPerspective = perspective(M_PI_2, 1, shadowMapNear, shadowMapFar);

	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Here are the rotations for each face of the cubemap (please do use them!)

}

void RGLib::World::RenderWorldObjects()
{
	//int i is used to link world objects to their queries
	//TODO make world objects into their own mini class with queries attached or attach queries to meshes
	
	//shadowMap->RenderShadowMap(worldMeshes, shadowRec, worldLights[0]);

	int i = 0;
	for (glhelper::Mesh* mesh : worldMeshes)
	{
		GLuint currentQuery = NULL;//queries[i];
		i++;
		
		//check mesh has texture and bind it
		glActiveTexture(GL_TEXTURE0 + 0);
		if(mesh->meshTex != nullptr)
			mesh->meshTex->bindToImageUnit(0);
		//TODO else bind empty tex

		//store query data for each mesh rendered 
		glBeginQuery(GL_TIME_ELAPSED_EXT, currentQuery);
		mesh->render();
		glEndQuery(GL_TIME_ELAPSED_EXT);
	}

	for (WorldObject* object : worldObjects)
	{
		//check mesh has texture and bind it
		glActiveTexture(GL_TEXTURE0 + 0);
		if (object->getMesh()->meshTex != nullptr)
			object->getMesh()->meshTex->bindToImageUnit(0);
		//TODO else bind empty tex

		//bind normal map
		if (object->GetNormal() != NULL)
		{
			glDisable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, object->GetNormal());
		}


		//store query data for each mesh rendered 
		if (!object->IsQueryQueued())
		{
			glBeginQuery(GL_TIME_ELAPSED_EXT, object->GetQuery());
			object->getMesh()->render();
			glEndQuery(GL_TIME_ELAPSED_EXT);
			object->SetQueryQueued(true);
		}
		else
		{
			object->getMesh()->render();
		}
		//Unbind textures
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, NULL);

	}

	//for (auto& model : *models) {
	//	glhelper::Mesh& mesh = meshes.at(model["mesh"]);
	//	glhelper::ShaderProgram& shader = shaders.at(model["shader"]);
	//	glhelper::Texture& texture = textures.at(model["texture"]);
	//	texture.bindToImageUnit(0);
	//	glProgramUniform1i(shader.get(), shader.uniformLoc("tex"), 0);
	//	mesh.shaderProgram(&shader);

	//	Eigen::Vector3f position(model["position"][0], model["position"][1], model["position"][2]);
	//	mesh.modelToWorld(makeTranslationMatrix(position));
	//	mesh.render();
	//}



	//only print every 30 frames to make console readable in realtime
	//if (frameCount >= 30)
	//{
	//	//reset i to 0 to link queries
	//	i = 0;
	//	//total time rendering each mesh
	//	GLuint64 totalTime = 0;

	//	//for (GLuint& query : queries)
	//	//{
	//	//	GLint availible = 0;
	//	//	while (!availible)
	//	//	{
	//	//		glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &availible);
	//	//	}
	//	//	GLuint64 timeElapsed;
	//	//	glGetQueryObjectui64v(query, GL_QUERY_RESULT, &timeElapsed);
	//	//	totalTime += timeElapsed;
	//	//	//convert time to milliseconds for console output
	//	//	std::cout << "Mesh: " << worldMeshes[i]->meshName << " " << timeElapsed / 1e6f << "\n";
	//	//	//TODO change to show names on colums and data on rows for easy chart creation
	//	//	//dataFile << worldMeshes[i]->meshName << timeElapsed << ", ";
	//	//	i++;
	//	//}

	//	//to fps
	//	//std::cout << "Total Render Time: " << totalTime / 1e6f << "ms" << "\n";
	//	//GLuint64 fps;
	//	//fps = 1 / ((totalTime / 1e6f) / 1000.f);

	//	//print to console and convert frameDuration to fps
	//	std::cout << "Total FPS: " << std::to_string(1 / frameDuration) << "fps" << "\n";
	//	//dataFile << "Total mesh render time: " << totalTime << " FPS: " << std::to_string(1 / frameDuration) << "\n";

	//	gltSetText(fpsText, (std::string("FPS: ") + std::to_string(1 / frameDuration)).c_str());

	//	//dataFile.flush();
	//	frameCount = 0;
	//}


}


void RGLib::World::RenderWorldObjects(glhelper::ShaderProgram& shader)
{
	for (WorldObject* object : worldObjects)
	{
		//check mesh has texture and bind it
		glActiveTexture(GL_TEXTURE0 + 0);
		if (object->getMesh()->meshTex != nullptr)
			object->getMesh()->meshTex->bindToImageUnit(0);
		//TODO else bind empty tex

		//bind normal map
		if (object->GetNormal() != NULL)
		{
			glDisable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, object->GetNormal());
		}


			/*GLint availible = 0;
			while (!availible)
			{
				glGetQueryObjectiv(object->GetQuery(), GL_QUERY_RESULT_AVAILABLE, &availible);
			}
			GLuint64 timeElapsed;
			glGetQueryObjectui64v(object->GetQuery(), GL_QUERY_RESULT, &timeElapsed);*/
			//convert time to milliseconds for console output
			//std::cout << "Mesh: " << worldMeshes[i]->meshName << " " << timeElapsed / 1e6f << "\n";
			//TODO change to show names on colums and data on rows for easy chart creation
			//dataFile << worldMeshes[i]->meshName << timeElapsed << ", ";
		


		//store query data for each mesh rendered 

		object->getMesh()->render(shader);

		//Unbind textures
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

}
template<typename T, int m, int n>
inline glm::mat<m, n, float, glm::precision::highp> E2GLM(const Eigen::Matrix<T, m, n>& em)
{
	glm::mat<m, n, float, glm::precision::highp> mat;
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			mat[j][i] = em(i, j);
		}
	}
	return mat;
}
glm::mat4 test;
void RGLib::World::RenderReflectedObjects()
{
	glBeginQuery(GL_TIME_ELAPSED_EXT, queries["Reflections"]);
	for (WorldObject* object : worldObjects)
	{
		
		Eigen::Matrix4f mtwCache = object->getMesh()->modelToWorld();
		Eigen::Matrix4f pineconeMTW = Eigen::Matrix4f::Identity();
		test = glm::scale(glm::translate(E2GLM(object->getMesh()->modelToWorld())
			, glm::vec3(0, 0, -1)), glm::vec3(1, 1, -1));
		//Eigen::Scaling(makeTranslationMatrix(Eigen::Vector3f(0, 0, -1) * object->getMesh()->modelToWorld()), Eigen::Vector3f(1, 1, -1));
		object->getMesh()->modelToWorld(object->getMesh()->modelToWorld() * makeTranslationMatrix(Eigen::Vector3f(0, -0.1f, 0)) * makeRotationMatrix(0, -180, -180));
		//check mesh has texture and bind it
		glActiveTexture(GL_TEXTURE0 + 0);
		if (object->getMesh()->meshTex != nullptr)
			object->getMesh()->meshTex->bindToImageUnit(0);
		//TODO else bind empty tex

		//bind normal map
		if (object->GetNormal() != NULL)
		{
			glDisable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, object->GetNormal());
		}


		//store query data for each mesh rendered 
		object->getMesh()->render();
		//Unbind textures
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, NULL);
		object->getMesh()->modelToWorld(mtwCache);
	}
	glEndQuery(GL_TIME_ELAPSED_EXT);

}


void RGLib::World::RenderGUI()
{
	//draw text to window
	gltBeginDraw();
	gltColor(1.f, 1.f, 1.f, 1.f);
	gltDrawText2D(fpsText, 10.f, 10.f, 1.f);

	gltDrawText2D(frameCountText, 10.f, 40.f, 1.f);
	gltEndDraw();

}

void RGLib::World::CreateShadowMaps()
{
	shadowMap->CreateCubeMap(worldMeshes, worldLights[0]);
}

//Eigen::Matrix4f angleAxisMat4(float angle, const Eigen::Vector3f& axis)
//{
//	Eigen::Matrix4f output = Eigen::Matrix4f::Identity();
//	output.block<3, 3>(0, 0) = Eigen::AngleAxisf(angle, axis).matrix();
//	return output;
//}
//
//	const std::array<Eigen::Matrix4f, 6> cubemapRotations{
//		angleAxisMat4(float(M_PI_2), Eigen::Vector3f(0,1,0)),//POSITIVE_X - rotate right 90 degrees
//		angleAxisMat4(float(-M_PI_2), Eigen::Vector3f(0,1,0)),//NEGATIVE_X - rotate left 90 degrees
//		angleAxisMat4(float(-M_PI_2), Eigen::Vector3f(1,0,0)) * angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),//POSITIVE_Y - rotate up 90 degrees
//		angleAxisMat4(float(M_PI_2), Eigen::Vector3f(1,0,0)) * angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),//NEGATIVE_Y - rotate down 90 degrees
//		angleAxisMat4(float(M_PI), Eigen::Vector3f(0,1,0)),     //POSITIVE_Z - rotate right 180 degrees
//		Eigen::Matrix4f::Identity()                           //NEGATIVE_Z
//	};
void RGLib::World::RenderShadowMaps()
{
	
	//glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//glDisable(GL_CULL_FACE);
	//glViewport(0, 0, shadowMapSize, shadowMapSize);



	/*for (int i = 0; i < 6; ++i) {

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		Eigen::Matrix4f clipMatrix;

		clipMatrix = flipMatrix * cubemapPerspective * cubemapRotations[i] * makeTranslationMatrix(-worldLights[0]->GetPos());


		glProgramUniformMatrix4fv(shadowCubeMapShader->get(), shadowCubeMapShader->uniformLoc("shadowWorldToClip"), 1, false, clipMatrix.data());

		for (glhelper::Mesh* mesh : worldMeshes)
		{
			if (mesh->castsShadow())
				mesh->render(*shadowCubeMapShader);

		}

	}*/

	shadowMap->RenderShadowMap(worldMeshes, shadowRec, worldLights[0]);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, 1280, 720);

	//glActiveTexture(GL_TEXTURE0 + 1);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	//ground->render();
}

void RGLib::World::AddToWorld(glhelper::Mesh &mesh)
{
	worldMeshes.push_back(&mesh);

}

void RGLib::World::AddWorldObject(glhelper::Mesh& mesh)
{
	WorldObject* wO = new WorldObject(mesh);
	worldObjects.push_back(wO);
}

void RGLib::World::AddWorldObject(glhelper::Mesh& mesh, GLuint normalMap)
{
	WorldObject* wO = new WorldObject(mesh);
	wO->SetNormal(normalMap);
	worldObjects.push_back(wO);
}


void RGLib::World::AddWorldLight(RGLib::Light light)
{
	worldLights.push_back(&light);
}

void RGLib::World::CreateQueries()
{
	//create a query for each world object 
	//needs to be called after all world objects have been added
	for (glhelper::Mesh* mesh : worldMeshes)
	{
		GLuint query;
		glGenQueries(1, &query);
		//queries.push_back(query);
	}
	dataFile.clear();

	shadowMap = new RGLib::ShadowMap(512, worldLights[0]);
	shadowRec.push_back(ground);

	for (WorldObject* object : worldObjects)
	{
		dataFile << object->getName() << ", ";
	}

	queries["Reflections"] = NULL;
	queries["DepthofField"] = NULL;
	queries["Rain Particles"] = NULL;
	queries["Shadows"] = NULL;
	std::map <std::string, GLuint> ::iterator iter;
	for (iter = queries.begin(); iter != queries.end(); iter++)
	{
		GLuint query;
		glGenQueries(1, &query);
		(*iter).second = query;
		dataFile << (*iter).first << ", ";
	}
}
void RGLib::World::RecordQueries()
{
	bool allAvailible = true;


	for (WorldObject* object : worldObjects)
	{
		GLint availible = 0;

		glGetQueryObjectiv(object->GetQuery(), GL_QUERY_RESULT_AVAILABLE, &availible);

		if (!availible)
			allAvailible = false;
	}

	if (allAvailible)
	{
		dataFile << "\n";

		for (WorldObject* object : worldObjects)
		{
			//wait for query to become available 
			//while (!availible)
			//{
			//}
			GLuint64 timeElapsed;
			glGetQueryObjectui64v(object->GetQuery(), GL_QUERY_RESULT, &timeElapsed);
			dataFile << timeElapsed << ", ";
			object->SetQueryQueued(false);
		}
		for (std::pair pair : queries)
		{
			GLuint64 timeElapsed;
			glGetQueryObjectui64v(pair.second, GL_QUERY_RESULT, &timeElapsed);
			dataFile << timeElapsed << ", ";
		}
		frameSync++;
	}
	

	if (frameCount >= 30)
	{
	
		//print to console and convert frameDuration to fps
		std::cout << "Total FPS: " << std::to_string(1 / frameDuration) << "fps" << "\n";
		//dataFile << "Total mesh render time: " << totalTime << " FPS: " << std::to_string(1 / frameDuration) << "\n";

		gltSetText(fpsText, (std::string("FPS: ") + std::to_string(1 / frameDuration)).c_str());

		//dataFile.flush();
		frameCount = 0;
	}
	gltSetText(frameCountText, (std::string("Frame Sync: ") + std::to_string(frameSync)).c_str());

	frameCount++;

	//calculate duration per frame based upon time since last frame
	frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - lastFrameTime).count() * 1e-9f;
	lastFrameTime = std::chrono::steady_clock::now();
}


void RGLib::World::Clean()
{
	for (glhelper::Mesh* mesh : worldMeshes)
	{
		delete(&mesh->meshTex);
	}
	for (WorldObject* object : worldObjects)
	{
		delete(object);
	}
	//save data to file
	dataFile.close();
}

void RGLib::World::ClearWorld()
{
	worldMeshes.clear();
	queries.clear();
}



void RGLib::World::SetShadowMapShaders(glhelper::ShaderProgram &sMShader, glhelper::ShaderProgram &sCMShader)
{
	shadowMappedShader = &sMShader;
	shadowCubeMapShader = &sCMShader;
}
