#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Mesh.hpp"
#include "Constants.hpp"
#include "Exception.hpp"

namespace glhelper {
	//Source David Walton (Moodle.bcu.ac.uk)

Mesh::Mesh(const Eigen::Matrix4f &modelToWorld)
    :Renderable(modelToWorld),
    shaderProgram_(nullptr),
    nElems_(0), nVerts_(0),
	drawMode_(GL_TRIANGLES)
{
	glGenVertexArrays(1, &vao_);
}

Mesh::~Mesh() throw()
{
	glDeleteVertexArrays(1, &vao_);
}

void Mesh::vert(const std::vector<Eigen::Vector3f> &v, GLenum usage)
{
	vert_.reset(new VertexBuffer(v, usage));
	glBindVertexArray(vao_);
	vert_->bind();
	glEnableVertexAttribArray(UniformLocation::VERT);
	glVertexAttribPointer(UniformLocation::VERT, 3, GL_FLOAT, GL_FALSE, 0, 0);
	vert_->unbind();
	glBindVertexArray(0);
	nVerts_ = v.size();
	throwOnGlError();
}
void Mesh::norm(const std::vector<Eigen::Vector3f> &n, GLenum usage)
{
	norm_.reset(new VertexBuffer(n, usage));
	glBindVertexArray(vao_);
	norm_->bind();
	glEnableVertexAttribArray(UniformLocation::NORM);
	glVertexAttribPointer(UniformLocation::NORM, 3, GL_FLOAT, GL_FALSE, 0, 0);
	norm_->unbind();
	glBindVertexArray(0);
	throwOnGlError();
}
void Mesh::tex(const std::vector<Eigen::Vector2f> &n, GLenum usage)
{
	tex_.reset(new VertexBuffer(n, usage));
	glBindVertexArray(vao_);
	tex_->bind();
	glEnableVertexAttribArray(UniformLocation::TEX);
	glVertexAttribPointer(UniformLocation::TEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
	tex_->unbind();
	glBindVertexArray(0);
	throwOnGlError();
}
void Mesh::color(const std::vector<Eigen::Vector3f> &n, GLenum usage)
{
	color_.reset(new VertexBuffer(n, usage));
	glBindVertexArray(vao_);
	color_->bind();
	glEnableVertexAttribArray(UniformLocation::COLOR);
	glVertexAttribPointer(UniformLocation::COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
	color_->unbind();
	glBindVertexArray(0);
	throwOnGlError();
}
void Mesh::tangent(const std::vector<Eigen::Vector3f> &n, GLenum usage)
{
	tangent_.reset(new VertexBuffer(n, usage));
	glBindVertexArray(vao_);
	tangent_->bind();
	glEnableVertexAttribArray(UniformLocation::TANGENT);
	glVertexAttribPointer(UniformLocation::TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
	tangent_->unbind();
	glBindVertexArray(0);
	throwOnGlError();
}
void Mesh::bitangent(const std::vector<Eigen::Vector3f> &n, GLenum usage)
{
	bitangent_.reset(new VertexBuffer(n, usage));
	glBindVertexArray(vao_);
	bitangent_->bind();
	glEnableVertexAttribArray(UniformLocation::BITANGENT);
	glVertexAttribPointer(UniformLocation::BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
	bitangent_->unbind();
	glBindVertexArray(0);
	throwOnGlError();
}
void Mesh::elems(const std::vector<GLuint> &elems, GLenum usage)
{
	elem_.reset(new ElementBuffer(elems, usage));
	nElems_ = elems.size();
	glBindVertexArray(vao_);
	elem_->bind();
	glBindVertexArray(0);
	throwOnGlError();
}

void Mesh::render()
{
	if(shaderProgram_ == nullptr) {
		throw std::runtime_error("Attempted to render mesh without supplying shader program.");
	}
	glBindVertexArray(vao_);
	shaderProgram_->use();
	glProgramUniformMatrix4fv(shaderProgram_->get(), shaderProgram_->uniformLoc("modelToWorld"), 1, GL_FALSE, modelToWorld().data());
	glProgramUniformMatrix3fv(shaderProgram_->get(), shaderProgram_->uniformLoc("normToWorld"), 1, GL_FALSE, normToWorld().data());
	shaderProgram_->setupCameraBlock();
	if(nElems_ != 0) {
    	glDrawElements(drawMode_, GLsizei(nElems_), GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(drawMode_, 0, GLsizei(nVerts_));
	}
	shaderProgram_->unuse();
	glBindVertexArray(0);
}

//void Mesh::render(RGLib::Texture *texture)
//{
//	if (shaderProgram_ == nullptr) {
//		throw std::runtime_error("Attempted to render mesh without supplying shader program.");
//	}
//	glBindVertexArray(vao_);
//	shaderProgram_->use();
//	glProgramUniformMatrix4fv(shaderProgram_->get(), shaderProgram_->uniformLoc("modelToWorld"), 1, GL_FALSE, modelToWorld().data());
//	glProgramUniformMatrix3fv(shaderProgram_->get(), shaderProgram_->uniformLoc("normToWorld"), 1, GL_FALSE, normToWorld().data());
//	shaderProgram_->setupCameraBlock();
//
//	//select the texture
//	GLuint programId = 0;
//	GLint samplerID = glGetUniformLocation(programId, "albedoTex");
//	glActiveTexture(GL_TEXTURE0);
//	glUniform1i(samplerID, 0);
//	glBindTexture(GL_TEXTURE_2D, texture->getTextureName());
//
//	if (nElems_ != 0) {
//		glDrawElements(drawMode_, GLsizei(nElems_), GL_UNSIGNED_INT, 0);
//	}
//	else {
//		glDrawArrays(drawMode_, 0, GLsizei(nVerts_));
//	}
//	shaderProgram_->unuse();
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glBindVertexArray(0);
//}

void Mesh::render(ShaderProgram& program)
{
	glBindVertexArray(vao_);
	program.use();
	glProgramUniformMatrix4fv(program.get(), program.uniformLoc("modelToWorld"), 1, GL_FALSE, modelToWorld().data());
	glProgramUniformMatrix3fv(program.get(), program.uniformLoc("normToWorld"), 1, GL_FALSE, normToWorld().data());
	program.setupCameraBlock();
	if(nElems_ != 0) {
    	glDrawElements(drawMode_, GLsizei(nElems_), GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(drawMode_, 0, GLsizei(nVerts_));
	}
	program.unuse();
	glBindVertexArray(0);
}

bool Mesh::castsShadow()
{
	return castsShadow_;
}

void Mesh::setCastsShadow(bool value)
{
	castsShadow_ = value;
}

ShaderProgram *Mesh::shaderProgram() const
{
	return shaderProgram_;
}

Mesh& Mesh::shaderProgram(ShaderProgram *p)
{
	shaderProgram_ = p;
	return *this;
}

Mesh& Mesh::drawMode(GLenum mode)
{
	drawMode_ = mode;
	return *this;
}

void Mesh::loadTexture(cv::String filename)
{
	cv::Mat textureImage = cv::imread(filename);
	cv::cvtColor(textureImage, textureImage, cv::COLOR_BGR2RGB);


	 meshTex = std::make_unique< glhelper::Texture>(GL_TEXTURE_2D, GL_RGB8, textureImage.cols, textureImage.rows,
		0, GL_RGB, GL_UNSIGNED_BYTE, textureImage.data, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	meshTex->genMipmap();

}

void Mesh::setTexture(glhelper::Texture &texture)
{
	//meshTex = std::make_unique<glhelper::Texture>(GL_TEXTURE_2D, GL_RGB8, texture.width(), texture.height(),
	//	0, GL_RGB, GL_UNSIGNED_BYTE, texture.tex(), GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	//meshTex->genMipmap();
}


//end source
}
