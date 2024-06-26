#pragma once
#include "../ew/external/glad.h"

namespace livingstone
{
	struct Framebuffer
	{
		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat);
	Framebuffer createShadowMap(unsigned int width, unsigned int height, int colorFormat);
	Framebuffer createGbuffer(unsigned int width, unsigned int height);
}

