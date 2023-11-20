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
	for (glhelper::Mesh* mesh : worldObjects)
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
			std::cout << "Mesh: " << worldObjects[i]->meshName << " " << timeElapsed / 1e6f << "\n";
			//TODO change to show names on colums and data on rows for easy chart creation
			dataFile << worldObjects[i]->meshName << timeElapsed << ", ";
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
	gltBeginDraw();
	gltColor(1.f, 1.f, 1.f, 1.f);
	gltDrawText2D(fpsText, 10.f, 10.f, 1.f);
	gltEndDraw();
}

void RGLib::World::AddToWorld(glhelper::Mesh &mesh)
{
	worldObjects.push_back(&mesh);

}

void RGLib::World::CreateQueries()
{
	//create a query for each world object 
	//needs to be called after all world objects have been added
	for (glhelper::Mesh* mesh : worldObjects)
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
