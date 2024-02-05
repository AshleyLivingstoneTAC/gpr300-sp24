#include "framebuffer.h"

namespace livingstone
{
	Framebuffer::Framebuffer()
	{

	}

	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		unsigned int fbo, colorBuffer, depthBuffer;

		//Create Framebuffer Object
		    glCreateFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//Create 8 bit RGBA color buffer
		glGenTextures(1, &colorBuffer);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		//Attach color buffer to framebuffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer, 0);

		glGenTextures(1, &depthBuffer);
		glBindTexture(GL_TEXTURE_2D, depthBuffer);
		//Create 16 bit depth buffer - must be same width/height of color buffer
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		//Attach to framebuffer (assuming FBO is bound)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
		return Framebuffer();
	}
}