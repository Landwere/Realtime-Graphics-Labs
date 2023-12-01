#include "FrameBuffer.h"

//From moodle
namespace RGLib {
	FrameBuffer::~FrameBuffer() {
		glDeleteFramebuffers(1, &location);

	}

	int FrameBuffer::init() {
		GLuint null;
		glGenFramebuffers(1, &location);
		glBindFramebuffer(GL_FRAMEBUFFER, location);

		glGenTextures(1, &textureLocation);
		glBindTexture(GL_TEXTURE_2D, textureLocation);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		glGenRenderbuffers(1, &depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLocation, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return -1;

		glBindFramebuffer(GL_FRAMEBUFFER, location);
		glViewport(0, 0, width, height);

		/*glBindRenderbuffer(GL_RENDERBUFFER, null);
		glBindTexture(GL_TEXTURE_2D, null);
		glBindFramebuffer(GL_FRAMEBUFFER, null);*/
		
		return 0;
	}
}