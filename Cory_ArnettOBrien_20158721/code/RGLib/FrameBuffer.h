#pragma once

#include <GL/glew.h>

namespace RGLib {
	class FrameBuffer {
	public:
		FrameBuffer(int _width, int _height) : width(_width), height(_height) {}
		~FrameBuffer();

		int init();
	private:
		unsigned int location = 0, textureLocation = 0, depthRenderBuffer = 0;
		unsigned int VAO, IBO, VBO;
		int width, height;
	};
}