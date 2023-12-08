#include "World.hpp"
#include "World.hpp"
#include "World.hpp"



RGLib::World::World()
{
	gltInit();

	dataFile.open("renderData.csv", std::ios_base::app);
	fpsText = gltCreateText();

	frameCount = 0;
}

void RGLib::World::RenderWorld()
{
	//int i is used to link world objects to their queries
	//TODO make world objects into their own mini class with queries attached or attach queries to meshes
	int i = 0;
	for (glhelper::Mesh* mesh : worldMeshes)
	{
		GLuint currentQuery = queries[i];
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
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D,object->GetNormal());

		//store query data for each mesh rendered 
		object->getMesh()->render();
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
	if (frameCount >= 30)
	{
		//reset i to 0 to link queries
		i = 0;
		//total time rendering each mesh
		GLuint64 totalTime = 0;

		for (GLuint& query : queries)
		{
			GLint availible = 0;
			while (!availible)
			{
				glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &availible);
			}
			GLuint64 timeElapsed;
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, &timeElapsed);
			totalTime += timeElapsed;
			//convert time to milliseconds for console output
			std::cout << "Mesh: " << worldMeshes[i]->meshName << " " << timeElapsed / 1e6f << "\n";
			//TODO change to show names on colums and data on rows for easy chart creation
			dataFile << worldMeshes[i]->meshName << timeElapsed << ", ";
			i++;
		}

		//to fps
		std::cout << "Total Render Time: " << totalTime / 1e6f << "ms" << "\n";
		//GLuint64 fps;
		//fps = 1 / ((totalTime / 1e6f) / 1000.f);

		//print to console and convert frameDuration to fps
		std::cout << "Total FPS: " << std::to_string(1 / frameDuration) << "fps" << "\n";
		dataFile << "Total mesh render time: " << totalTime << " FPS: " << std::to_string(1 / frameDuration) << "\n";

		gltSetText(fpsText, (std::string("FPS: ") + std::to_string(1 / frameDuration)).c_str());

		//dataFile.flush();
		frameCount = 0;
	}

	frameCount++;

	//calculate duration per frame based upon time since last frame
	frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - lastFrameTime).count() * 1e-9f;
	lastFrameTime = std::chrono::steady_clock::now();


	//draw text to window
	/*gltBeginDraw();
	gltColor(1.f, 1.f, 1.f, 1.f);
	gltDrawText2D(fpsText, 10.f, 10.f, 1.f);
	gltEndDraw();*/
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

void RGLib::World::CreateQueries()
{
	//create a query for each world object 
	//needs to be called after all world objects have been added
	for (glhelper::Mesh* mesh : worldMeshes)
	{
		GLuint query;
		glGenQueries(1, &query);
		queries.push_back(query);
	}
	dataFile.clear();

}



void RGLib::World::Clean()
{
	//save data to file
	dataFile.close();
}

void RGLib::World::ClearWorld()
{
	worldMeshes.clear();
	queries.clear();
}
