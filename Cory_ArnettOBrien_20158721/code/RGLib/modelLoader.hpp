#pragma once
#include <GL/glew.h>
#include "RGLib/Mesh.hpp"
#include <iostream>
// source from Cory Arnett-O'Brien (Game Engine Architecture)


	struct Vertex
	{
		//location
		float x, y, z;
		float u, v;
		float nx, ny, nz;

		Vertex(float _x, float _y, float _z, float _u, float _v, float _nx, float _ny, float _nz)
		{
			x = _x;
			y = _y;
			z = _z;

			//Colour
			u = _u;
			v = _v;

			nx = _nx;
			ny = _ny;
			nz = _nz;
		}

		//set the vertex to origin and no colour
		Vertex()
		{
			x = y = z = 0.0f;
			u = v = 0.0f;
		}
	};

	class Model
	{
	public:
		Model()
		{
			vbo = 0;
			numVertices = 0;
		}

		~Model()
		{
			glDeleteBuffers(1, &vbo);
		}

		//help method to load vertices from file
		bool loadFromFile(const std::string& filename, glhelper::Mesh* mesh);

		//returns the vertices so we can create a vertex buffer
		//based on the model vertices
		GLuint getVbo()
		{
			return vbo;
		}

		GLuint getIbo()
		{
			return ibo;
		}
		//returns the number of vertices in model
		//need this for rendering as OPegnGL needs
		//to know the number of vertices to render
		int getNumVertices()
		{
			return numVertices;
		}

		GLuint getIndexCount()
		{
			return indexCount;
		}

		bool hasIndices = false;

	protected:
		GLuint vbo;
		GLuint ibo;
		GLuint indexCount;
		int numVertices;
	};

	//end source

