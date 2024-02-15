#include "framebuffer.h"
#include <glm/glm.hpp>
#include "external/glad.h"
#include "external/stb_image.h"

	livingstone::Framebuffer livingstone::createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		
		livingstone::Framebuffer buffer;
		
		//Create Framebuffer Object
		    glCreateFramebuffers(1, &buffer.fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		//Create 8 bit RGBA color buffer
		glGenTextures(1, &buffer.colorBuffer[0]);
		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		//Attach color buffer to framebuffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer.colorBuffer[0], 0);

		glGenTextures(1, &buffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
		//Create 16 bit depth buffer - must be same width/height of color buffer
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		//Attach to framebuffer (assuming FBO is bound)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);

		glGenTextures(1, &buffer.shadowMap);
		glBindTexture(GL_TEXTURE_2D, buffer.shadowMap);
		//16 bit depth values, 2k resolution 
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, 2048, 2048);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//Pixels outside of frustum should have max distance (white)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);


		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
		buffer.width = width, buffer.height = height;
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer incomplete: %d", status);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return buffer;
	}
