#include "World.hpp"



void RGLib::World::RenderWorld()
{
	dataFile.open("renderData.csv", std::ios_base::app);

	int i = 0;
	for (glhelper::Mesh* mesh : worldObjects)
	{
		GLuint currentQuery = queries[i];
		i++;
		
		glBeginQuery(GL_TIME_ELAPSED_EXT, currentQuery);
		mesh->render();
		glEndQuery(GL_TIME_ELAPSED_EXT);
	}

	i = 0;
	for (GLuint &query : queries)
	{
		GLint availible = 0;
		while (!availible)
		{
			glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &availible);
		}
		GLuint64 timeElapsed;
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &timeElapsed);
		std::cout << "Mesh: " << worldObjects[i]->meshName << " " << timeElapsed << std::endl;
		dataFile << worldObjects[i]->meshName << timeElapsed << " ";
		i++;
	}
	dataFile.close();

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
