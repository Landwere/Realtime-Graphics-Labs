#pragma once

#include <GL/glew.h>

namespace RGLib {
	/*! \brief Previously used for FrameBuffer reflections, now unused
	* 
	* Based on code from Game Engine Architecture
	*/
	class FrameBuffer {
	public:
		FrameBuffer(int _width, int _height) : width(_width), height(_height) {}
		~FrameBuffer();


		unsigned int getTextureLocation() { return textureLocation; }
		void bind() { glBindFramebuffer(GL_FRAMEBUFFER, location); }
		void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		int init();
	private:
		unsigned int location = 0, textureLocation = 0, depthRenderBuffer = 0;
		unsigned int VAO, IBO, VBO;
		int width, height;
	};
}