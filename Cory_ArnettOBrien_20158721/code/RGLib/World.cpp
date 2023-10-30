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
		mesh->meshTex->bindToImageUnit(0);

		glBeginQuery(GL_TIME_ELAPSED_EXT, currentQuery);
		mesh->render();
		glEndQuery(GL_TIME_ELAPSED_EXT);
	}
	if (frameCount >= 900)
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
			std::cout << "Mesh: " << worldObjects[i]->meshName << " " << timeElapsed / 10000 << "\n";
			dataFile << worldObjects[i]->meshName << timeElapsed << " ";
			i++;
		}
		//to fps
		std::cout << "Total Render Time: " << totalTime / 10000 << "ms" << "\n";
		GLuint64 fps;
		fps = 1 / ((totalTime / 10000.f) / 1000.f);
		std::cout << "Total FPS: " << fps << "fps" << "\n";
		//dataFile.flush();
		gltSetText(fpsText, (std::string("FPS: ") + std::to_string(fps)).c_str());
		frameCount = 0;
	}
	gltBeginDraw();
	gltColor(1.f, 1.f, 1.f, 1.f);
	gltDrawText2D(fpsText, 10.f, 10.f, 1.f);
	gltEndDraw();
	frameCount++;
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
