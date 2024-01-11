#pragma once

#include <GL/glew.h>
#include <string>

namespace glhelper {
	
	
	//Source David Walton (Moodle.bcu.ac.uk)

//!\brief Class encapsulating a (2D) OpenGL texture.
class Texture final
{
public:
	explicit Texture(GLenum target, GLenum internalFormat, 
		size_t width, size_t height, 
		GLint border, GLenum format, GLenum type,
		const GLvoid *data = nullptr,
		GLenum minFilter = GL_NEAREST,
		GLenum magFilter = GL_NEAREST);
	~Texture() throw();
	Texture(Texture &&);
	Texture(Texture&);
	Texture() {};
	Texture &operator=(Texture&&);
	Texture &operator=(Texture&);

	void update(const void *data);
	void update(const void *data, size_t mipmapLevel);

	void bindToImageUnit(GLuint unit);
	void unbind();

	GLuint tex();

	void getData(void *data, size_t buffSize);
	void getData(void *data, size_t mipmapLevel, size_t buffSize);
	unsigned char* getDataP();
	void saveToFile(const std::string &filepath);

	size_t width() const;
	size_t height() const;
	size_t maxLevels() const;
	size_t nTexels() const;
	float aspect() const;
	size_t bytesPerPixel() const;
	size_t numChannels() const;
	size_t bytesPerChannel() const;

	void genMipmap();

	GLuint tex_;

private:
	Texture(const Texture&);
	Texture& operator=(const Texture&);

	void init(const void *data, GLenum minFilter, GLenum magFilter);
	unsigned char* pixelData;
	size_t width_, height_;
	GLenum target_, internalFormat_, format_, type_, border_;
	
	friend class CubemapTexture;
};
//end source
}

