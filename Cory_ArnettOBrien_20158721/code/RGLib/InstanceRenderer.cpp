#include "InstanceRenderer.hpp"
#include "ShaderProgram.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

//Based on code from game engine architecture 
namespace RGLib {

	InstanceRenderer::InstanceRenderer() {
	}

	InstanceRenderer::~InstanceRenderer()
	{
		// 13. Delete the instance buffer when finished


		glDeleteProgram(programId);
	}



	void InstanceRenderer::init()
	{
		//// New! Load shader source from files.  Need the new ShaderUtils files
		//std::string v_shader_source = loadShaderSourceCode("instance.vert");
		//std::string f_shader_source = loadShaderSourceCode("instance.frag");

		//// Due to the unique way OpenGL handles shader source, OpenGL expects
		//// an array of strings.  In this case, create an array of the
		//// loaded source code strings and pass to compileProgram for compilation
		//const GLchar* v_source_array[] = { v_shader_source.c_str() };
		//const GLchar* f_source_array[] = { f_shader_source.c_str() };

		//// Compile shaders into a program
		//if (!compileProgram(v_source_array, f_source_array, &programId)) {
		//	std::cerr << "Problem building billboard program.  Check console log for more information." << std::endl;
		//}
		glhelper::ShaderProgram instanceShader({ "..\\shaders\\instance.vert", "..\\shaders\\instance.frag" });
		programId = instanceShader.get();
		// Now get a link to the vertexPos3D so we can link the attribute
		// to our vertices when rendering
		vertexLocation = glGetAttribLocation(programId, "vertexPos3D");

		// Check for errors
		if (vertexLocation == -1) {
			std::cerr << "Problem getting vertex3DPos" << std::endl;
		}

		vertexUVLocation = glGetAttribLocation(programId, "vUV");

		// Check for errors
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		// 2. Get the location of the instance attribute
		// which is the transformation matrix.  
		instanceMatLocation = glGetAttribLocation(programId, "instance_transform_mat");
		//check for errors
		if (instanceMatLocation == -1) {
			std::cerr << "Problem getting instance transform mat" << std::endl;
		}

		// Link the uniforms to the member fields
		viewUniformId = glGetUniformLocation(programId, "view");
		projectionUniformId = glGetUniformLocation(programId, "projection");
		samplerId = glGetUniformLocation(programId, "sampler");
	}

	void InstanceRenderer::setInstanceData(const std::vector<InstancePosRotScale>& instances) {
		// 3. Create a vector to store the transformation matrices
		std::vector<glm::mat4> instance_matrices;

		// 4. Process the location, rotation and scales from instances vector
		// calculating the corresponding transformation matrix and insert
		// into vector
		for (auto& iloc : instances)
		{
			// Calculate the transformation matrix for the object.  Start with the identity matrix
			glm::mat4 instanceMat = glm::mat4(1.0f);

			// Current instance position, rotation and scale
			instanceMat = glm::translate(instanceMat, glm::vec3(iloc.posX, iloc.posY, iloc.posZ));
			instanceMat = glm::rotate(instanceMat, glm::radians(iloc.rotX), glm::vec3(1.0f, 0.0f, 0.0f));
			instanceMat = glm::rotate(instanceMat, glm::radians(iloc.rotY), glm::vec3(0.0f, 1.0f, 0.0f));
			instanceMat = glm::rotate(instanceMat, glm::radians(iloc.rotZ), glm::vec3(0.0f, 0.0f, 1.0f));

			instance_matrices.push_back(instanceMat);
		}



		// Store the number of instances as need that number in
		// draw method
		numInstances = instance_matrices.size();

		// 5. Setup input of an array of matrix which define the position of each instance
		glGenBuffers(1, &instanceMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, instance_matrices.size() * sizeof(glm::mat4), instance_matrices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Draw objects using instancing
	void InstanceRenderer::drawInstanced(Camera* cam, glhelper::Mesh* m) {
		// Enable back face culling by enabling face culling
		// Note that back face culling is the default culling
		// Change cull type with glCullFace() and pass either
		// GL_FRONT, GL_BACK or GL_FRONT_AND_BACK
		glEnable(GL_CULL_FACE);

		// Get the view and projection matrices
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Select the vertex buffer object into the context
		//glBindBuffer(GL_ARRAY_BUFFER, m->getVertices());
		m->getVbo()->bind();

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos2DLocation attribute
		//glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(m->getTbo()), (void*)offsetof(m->getTbo(), x));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);

		// Define where the vertex specification will find the colour data and how much
		// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
		//glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(m->getTbo()), (void*)offsetof(m->getTbo(), u));

		// 6. Unbind the vertex buffer before defining the input for the
		// transformation matrices instance buffer


		// 7. Now bind the matrix to the input specification which starts at instance_transform_mat


		// 8. Define the layout of the instance matrix input attribute in terms of
		// four vec4 values.  Reason is can't define an input in more than a vec4 so
		// matrix has be broken down into four vec4s


		// 9. Define how the instance data is used in terms of how many instances
		// pass before the transformation matrix is updated.  1 means one instance
		// or one pass of all of the vertices in the model.  2 means after two
		// passes and so on.  0 means every vertex which is normal behaviour


		// 10. Unbind the instance buffer as may want to define more inputs
		// based on another buffer


		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, m->meshTex->tex());

		// 11. Draw the instances


		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);
		// 12. Unselect the attribute from the context


		// Unselect the program from the context
		glUseProgram(0);

		glDisable(GL_CULL_FACE);
	}


}