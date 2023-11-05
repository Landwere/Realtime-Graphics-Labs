#include "ModelRenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.hpp"
#include "ShaderUtils.h"
	namespace RGLib 
	{
		//Source Cory Arnett-O'Brien (Game engine architecture)

		//GLfloat vertexData[] =
		//{
		//	-1.0f, 0.0f,
		//	1.0f, 0.0f,
		//	0.0f, 1.0f
		//};
		//vertices
		//Vertex triangleVertices[] = {
		//	//Vertex(-1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f),
		//	//Vertex(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		//	//Vertex(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f)
		//};

		ModelRenderer::ModelRenderer()
		{
			pos_x = pos_y = pos_z = 0.0f;
			rot_x = rot_y = rot_z = 0.0f;
			scale_x = scale_y = scale_z = 1.0f;

			programId = 0;

			vertexLocation = 0;

			vertexUVLocation = 0;

			transformUniformId = 0;
			viewUniformId = 0;
			projectionUniformId = 0;


			//ShaderProgram model_shader({ "../shaders/VertexLighting.vert", "../shaders/FragmentLighting.frag" });
			//ShaderProgram f_shader({ "../shaders\\FragmentLighting.frag" });


			ShaderUtils modelShader;
			modelShader.loadShaderSourceCode(".. / shaders\\VertexLighting.vert", "../shaders\\FragmentLighting.frag");

			modelShader.compileProgram(modelShader.v_shader_source, modelShader.v_shader_source, &programId);

			//ShaderUtils v_shader;
			 //std::string v_shader_source = v_shader.loadShaderSourceCode("../shaders\\VertexLighting.vert");

			//ShaderUtils f_shader;
			 //std::string f_shader_source = f_shader.loadShaderSourceCode("../shaders\\FragmentLighting.frag");

			//std::string v_shader_source = loadShaderSourceCode("../shaders\\VertexLighting.vert");
			//std::string f_shader_source = loadShaderSourceCode("../shaders\\FragmentLighting.frag");

			// Due to the unique way OpenGL handles shader source, OpenGL expects
			// an array of strings.  In this case, create an array of the
			// loaded source code strings and pass to compileProgram for compilation
			//const GLchar* v_source_array[] = { v_shader_source.c_str() };
			//const GLchar* f_source_array[] = { f_shader_source.c_str() };

			 //Compile shaders into a program
			/*if (!v_shader.compileProgram(v_shader_source, f_shader_source, &programId)) {
				std::cerr << "Problem building lighting shader program.  Check console log for more information." << std::endl;
			}*/



			//now get a link to the vertexpos2D so we can link the attribute
			//to our vertcies when rendering
			vertexLocation = glGetAttribLocation(programId, "vertexPosition");

			//check for errors
			if (vertexLocation == -1)
			{
				std::cerr << "Problem getting VertexPos3D" << std::endl;
			}

			vertexUVLocation = glGetAttribLocation(programId, "uvCoordinates");
			if (vertexUVLocation == -1)
			{
				std::cerr << "Problem getting vUV" << std::endl;
			}
			vertexNormalLocation = glGetAttribLocation(programId, "vertexNormal");
			if (vertexNormalLocation == -1)
			{
				std::cerr << "Problem getting vNormal" << std::endl;
			}

			//link the uniforms to the member fields 
			transformUniformId = glGetUniformLocation(programId, "transform");
			viewUniformId = glGetUniformLocation(programId, "view");
			projectionUniformId = glGetUniformLocation(programId, "projection");
			samplerID = glGetUniformLocation(programId, "textureSampler");
			lightColour = glGetUniformLocation(programId, "lightColour");
			lightPosition = glGetUniformLocation(programId, "lightPosition");
			viewPosition = glGetUniformLocation(programId, "viewPosition");

			//link the fog uniforms
			fogColourId = glGetUniformLocation(programId, "fog_colour");
			fogStartId = glGetUniformLocation(programId, "fog_start");
			fogRangeId = glGetUniformLocation(programId, "fog_range");


		}

		ModelRenderer::~ModelRenderer()
		{

		}

		void ModelRenderer::init()
		{
					
			

		}

		void ModelRenderer::draw(Camera* cam, Model *model, Texture *texture)
		{
			glEnable(GL_CULL_FACE);
			//select the program into the rendering context
			glUseProgram(programId);

			material = texture;

			//calculate the transformation matrix for the object. start with the identity matrix 
			glm::mat4 transformationMat = glm::mat4(1.0f);

			transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
			transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
			transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
			transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
			transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

			//get the view and projections matrices
			glm::mat4 viewMat = cam->getViewMatrix();
			glm::mat4 projectionMat = cam->getProjectionMatrix();

			//set the unifroms in the shader
			glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
			glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

			//select the vertex buffer object into the context
			glBindBuffer(GL_ARRAY_BUFFER, model->getVbo());


			//enable the attribute to be passed vertices fro m the vertex buffer object  
			glEnableVertexAttribArray(vertexLocation);

			//define the structure of a vertex for OpenGL to select values from the vertex buffer
			//and store in vertexPos2DLocation attribute
			glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, x));



			
			//enable the attribute to be passed vertices from the vertec buffer object
			glEnableVertexAttribArray(vertexUVLocation);
			glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, u));

			glEnableVertexAttribArray(vertexNormalLocation);
			glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

			glUniform3f(lightColour, 255.0f / 255.0f, 200.0f / 255.0f, 200 / 255.0f);
			glUniform3f(lightPosition, 0.0f, 1000.0f, 6000.0f);
			glUniform3f(viewPosition, cam->getPosX(), cam->getPosY(), cam->getPosZ());


			//set fog values in init function
			glUseProgram(programId);
			glUniform1f(fogStartId, 200.0f);
			glUniform1f(fogRangeId, 500.0f);

			glm::vec3 fog_colour = glm::vec3(0.5f, 0.5f, 0.5f);
			glUniform3fv(fogColourId, 1, glm::value_ptr(fog_colour));

			//select the texture
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(samplerID, 0);
			//glBindTexture(GL_TEXTURE_2D, material->getTextureName());

			//draw the model 
			if (model->hasIndices == true) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getIbo());
				glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			}
			else {
				glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());
			}

			//unselect the attribute from the context
			glDisableVertexAttribArray(vertexLocation);
			glDisableVertexAttribArray(vertexUVLocation);
			glDisableVertexAttribArray(vertexNormalLocation);
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//unselect the	program from the context ]
			glUseProgram(0);

			glDisable(GL_CULL_FACE);


		}
		void ModelRenderer::draw(Camera* cam, Model* model, unsigned int textureLocation)
		{
			glEnable(GL_CULL_FACE);
			//select the program into the rendering context
			glUseProgram(programId);

			//material = textureLocation;

			//calculate the transformation matrix for the object. start with the identity matrix 
			glm::mat4 transformationMat = glm::mat4(1.0f);

			transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
			transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
			transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
			transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
			transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

			//get the view and projections matrices
			glm::mat4 viewMat = cam->getViewMatrix();
			glm::mat4 projectionMat = cam->getProjectionMatrix();

			//set the unifroms in the shader
			glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
			glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

			//select the vertex buffer object into the context
			glBindBuffer(GL_ARRAY_BUFFER, model->getVbo());


			//enable the attribute to be passed vertices fro m the vertex buffer object  
			glEnableVertexAttribArray(vertexLocation);

			//define the structure of a vertex for OpenGL to select values from the vertex buffer
			//and store in vertexPos2DLocation attribute
			glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));




			//enable the attribute to be passed vertices from the vertec buffer object
			glEnableVertexAttribArray(vertexUVLocation);
			glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

			glEnableVertexAttribArray(vertexNormalLocation);
			glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

			glUniform3f(lightColour, 1, 0.6, 0.6);
			glUniform3f(lightPosition, 0.0f, 1000.0f, 6000.0f);
			glUniform3f(viewPosition, cam->getPosX(), cam->getPosY(), cam->getPosZ());


			//set fog values in init function
			glUseProgram(programId);
			glUniform1f(fogStartId, 5000.0f);
			glUniform1f(fogRangeId, 8000.0f);

			glm::vec3 fog_colour = glm::vec3(0.5f, 0.5f, 0.5f);
			glUniform3fv(fogColourId, 1, glm::value_ptr(fog_colour));

			//select the texture
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(samplerID, 0);
			glBindTexture(GL_TEXTURE_2D, textureLocation);

			//draw the model 
			if (model->hasIndices == true) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getIbo());
				glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			}
			else {
				glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());
			}

			//unselect the attribute from the context
			glDisableVertexAttribArray(vertexLocation);
			glDisableVertexAttribArray(vertexUVLocation);
			glDisableVertexAttribArray(vertexNormalLocation);
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//unselect the	program from the context ]
			glUseProgram(0);

			glDisable(GL_CULL_FACE);

		}
		void ModelRenderer::clean()
		{
			glDeleteProgram(programId);

			glDeleteBuffers(1, &vboTriangle);
		}

		void ModelRenderer::update()
		{

		}

		void ModelRenderer::displayShaderCompilerError(GLuint shaderId)
		{
			//get the lenght of the error messsege
			GLint MsgLen = 0;

			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

			if (MsgLen > 1)
			{
				//create a character buffer to store the error messege characters
				GLchar* Msg = new GLchar[MsgLen + 1];

				//Get the error messege characters from the OpenGL internal log
				//into the Msg buffer
				glGetShaderInfoLog(shaderId, MsgLen, NULL, Msg);

				//Display the error messege 
				std::cerr << "Error with compiling shader" << Msg << std::endl;

				//release the memory acclocated to the string
				delete[] Msg;
			}
		}
		//end source
}