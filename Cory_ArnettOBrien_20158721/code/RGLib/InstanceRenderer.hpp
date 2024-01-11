#pragma once
#include <GL/glew.h>
#include <vector>
#include "Camera.h"
#include "Mesh.hpp"

namespace RGLib {
	/*! \brief helper class for InstanceRenderer */
	struct InstancePosRotScale {
		float posX, posY, posZ;
		float rotX, rotY, rotZ;
		float scaleX, scaleY, scaleZ;
	};
/*! \brief unused and incomplete class for instance rendering
*
* Based on code from game engine architecture
*/
	class InstanceRenderer
	{
	public:
		InstanceRenderer();
		~InstanceRenderer();
		void init();

		void drawInstanced(Camera* cam, glhelper::Mesh* m);

		void setInstanceData(const std::vector<InstancePosRotScale>& instances);

	private:
		// Member fields
		// This member stores the program object that contains the shaders
		GLuint programId;
		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexLocation;
		// Link to vColour attribute which receives a colour
		// and passes to fColour for fragment shader
		GLint vertexUVLocation;

		// Link instance matrix attribute
		GLint instanceMatLocation;

		// GLSL uniform variables for the transformation, view and projection matrices
		GLuint transformUniformId;
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;

		int numInstances;

		GLuint instanceMatrixBuffer;
	};

}