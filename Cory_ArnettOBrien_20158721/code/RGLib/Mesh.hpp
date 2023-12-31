#pragma once

#include "Renderable.hpp"
#include "GLBuffer.hpp"
#include <GL/glew.h>
#include "RGLib/Texture.hpp"
#include <opencv2/opencv.hpp>
namespace glhelper {
	//Source David Walton (Moodle.bcu.ac.uk)

//!\brief Class abstracting a renderable 3D triangle mesh.
class Mesh : public Renderable
{
public:
	Mesh(const Eigen::Matrix4f &modelToWorld = Eigen::Matrix4f::Identity());
	~Mesh() throw();

	void vert (const std::vector<Eigen::Vector3f> &verts, GLenum usage = GL_STATIC_DRAW);
	void norm (const std::vector<Eigen::Vector3f> &norms, GLenum usage = GL_STATIC_DRAW);
	void tex  (const std::vector<Eigen::Vector2f> &tex  , GLenum usage = GL_STATIC_DRAW);
	void color(const std::vector<Eigen::Vector3f> &colors, GLenum usage = GL_STATIC_DRAW);
	void tangent(const std::vector<Eigen::Vector3f> &tangents, GLenum usage = GL_STATIC_DRAW);
	void bitangent(const std::vector<Eigen::Vector3f> &bitangents, GLenum usage = GL_STATIC_DRAW);
	void elems(const std::vector<GLuint> &elems, GLenum usage = GL_STATIC_DRAW);


	virtual void render();
	//void render(RGLib::Texture* texture);
	virtual void render(ShaderProgram &program);

	virtual bool castsShadow();

	void setCastsShadow(bool);

	Mesh& shaderProgram(ShaderProgram *p);
	ShaderProgram* shaderProgram() const;

	Mesh& drawMode(GLenum mode);

	void loadTexture(cv::String filename);

	void setTexture(glhelper::Texture &texture);

	VertexBuffer* getVbo()
	{
		return vert_.get();
	}

	VertexBuffer* getTbo()
	{
		return tex_.get();
	}

	std::string meshName;

	std::unique_ptr< glhelper::Texture> meshTex;

private:
	Mesh(const Mesh&);
	Mesh& operator=(const Mesh&);
	std::unique_ptr<VertexBuffer> vert_, norm_, tex_, color_, tangent_, bitangent_;
	std::unique_ptr<ElementBuffer> elem_;
	ShaderProgram *shaderProgram_;
	//vertex array stores information about how objects are bound in the buffer
	//is used to set up vertex attributes 
	//sort of a vertex layout object
	GLuint vao_;
	size_t nElems_, nVerts_;
	GLenum drawMode_;
	bool castsShadow_;
};
//end source
}

