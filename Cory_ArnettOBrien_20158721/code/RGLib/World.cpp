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

	int i = 0;
	for (glhelper::Mesh* mesh : worldObjects)
	{
		GLuint currentQuery = queries[i];
		i++;
		

		glActiveTexture(GL_TEXTURE0 + 0);
		if(mesh->meshTex != nullptr)
			mesh->meshTex->bindToImageUnit(0);

		glBeginQuery(GL_TIME_ELAPSED_EXT, currentQuery);
		mesh->render();
		glEndQuery(GL_TIME_ELAPSED_EXT);
	}
	if (frameCount >= 30)
	{
		i = 0;
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
			std::cout << "Mesh: " << worldObjects[i]->meshName << " " << timeElapsed / 1e6f << "\n";
			dataFile << worldObjects[i]->meshName << timeElapsed << ", ";
			i++;
		}

		//to fps
		std::cout << "Total Render Time: " << totalTime / 1e6f << "ms" << "\n";
		GLuint64 fps;
		fps = 1 / ((totalTime / 1e6f) / 1000.f);
		std::cout << "Total FPS: " << std::to_string(1 / frameDuration) << "fps" << "\n";
		dataFile << "Total render time: " << totalTime << " FPS: " << std::to_string(1 / frameDuration) << "\n";

		gltSetText(fpsText, (std::string("FPS: ") + std::to_string(1 / frameDuration)).c_str());

		//dataFile.flush();
		frameCount = 0;
	}

	frameCount++;

	frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - lastFrameTime).count() * 1e-9f;
	lastFrameTime = std::chrono::steady_clock::now();



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
	dataFile.close();
}
