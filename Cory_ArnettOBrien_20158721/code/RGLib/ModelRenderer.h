#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <iostream>
#include "Camera.h"
#include <opencv2/opencv.hpp>
#include "TextureLegacy.h"

namespace RGLib
{
	//source Cory Arnett-O'Brien (Game Engine Architecture)
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
		bool loadFromFile(const char* filename);

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

class ModelRenderer
{
public:
	ModelRenderer();
	virtual ~ModelRenderer();

	void init();

	void update();

	void draw(Camera* cam, Model *model, Texture* texture);
	void draw(Camera* cam, Model* model, unsigned int textureLocation);

	void clean();

	void displayShaderCompilerError(GLuint shaderId);

	float getPosX()
	{
		return pos_x;
	}

	float getPosY()
	{
		return pos_y;
	}

	float getPosZ()
	{
		return pos_z;
	}

	float getRotX()
	{
		return rot_x;
	}

	float getRotY()
	{
		return rot_y;
	}

	float getRotZ()
	{
		return rot_z;
	}

	float getScaleX()
	{
		return scale_x;
	}

	float getScaleY()
	{
		return scale_y;
	}

	float getScaleZ()
	{
		return scale_z;
	}

	//mutator methods
	void setPos(float x, float y, float z)
	{
		pos_x = x;
		pos_y = y;
		pos_z = z;
	}

	void setRotation(float rx, float ry, float rz)
	{
		rot_x = rx;
		rot_y = ry;
		rot_z = rz;
	}

	void setScale(float sx, float sy, float sz)
	{
		scale_x = sx;
		scale_y = sy;
		scale_z = sz;
	}

	void setMaterial(Texture* mat)
	{
		material = mat;
	}
private:
	//Member fields
	//This member stores the program object that contains the shaders
	GLuint programId;

	//this member stores the attribute to select into the pipline 
	//to link the trangle vertcices to the pipleine 
	GLint vertexLocation;

	//link to vColour attribute which recives a colour
	//and passes to fColour for fragment shaders
	GLint vertexUVLocation;

	GLint vertexNormalLocation;

	//this member stores the triangle vertex buffer object containing the vertcies 
	//transferrd from this code to the graphics memory
	GLuint vboTriangle;

	Texture* material;

	float pos_x, pos_y, pos_z;
	float rot_x, rot_y, rot_z;
	float scale_x, scale_y, scale_z;

	//GLSL uniform variables for transformation view and projections matrices 
	GLint transformUniformId;
	GLint viewUniformId;
	GLint projectionUniformId;
	GLint samplerID;
	GLint lightColour;
	GLint lightPosition;
	GLint viewPosition;

	GLuint fogColourId;
	GLuint fogStartId;
	GLuint fogRangeId;

};

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

//end source
}

