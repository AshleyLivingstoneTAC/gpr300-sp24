#include "framebuffer.h"

void createFramebuffer()
{
	unsigned int fbo, colorBuffer;
	//Create Framebuffer Object
	glCreateFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//Create 8 bit RGBA color buffer
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	//Attach color buffer to framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer, 0);
}
