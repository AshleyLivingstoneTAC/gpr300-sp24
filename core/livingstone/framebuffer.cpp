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

		

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
		buffer.width = width, buffer.height = height;
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer incomplete: %d", status);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return buffer;
	}

	livingstone::Framebuffer livingstone::createShadowMap(unsigned int width, unsigned int height, int colorFormat)
	{
		livingstone::Framebuffer shadowBuffer;
		glCreateFramebuffers(1, &shadowBuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer.fbo);

		glGenTextures(1, &shadowBuffer.depthBuffer); 
		glBindTexture(GL_TEXTURE_2D, shadowBuffer.depthBuffer); 
		//16 bit depth values, 2k resolution 
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//Pixels outside of frustum should have max distance (white)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowBuffer.depthBuffer, 0); 
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		shadowBuffer.width = width, shadowBuffer.height = height; 
		return shadowBuffer;
	}

	livingstone::Framebuffer livingstone::createGbuffer(unsigned int width, unsigned int height)
	{
		Framebuffer framebuffer;
		framebuffer.width = width;
		framebuffer.height = height;

		glCreateFramebuffers(1, &framebuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

		int formats[3] = {
			GL_RGB32F, //0 = World Position 
			GL_RGB16F, //1 = World Normal
			GL_RGB16F  //2 = Albedo

		};
		for (size_t i = 0; i < 3; i++)
		{
			glGenTextures(1, &framebuffer.colorBuffer[i]);
			glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffer[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, formats[i], width, height);
			//Clamp to border so we don't wrap when sampling for post processing
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//Attach each texture to a different slot.
		//GL_COLOR_ATTACHMENT0 + 1 = GL_COLOR_ATTACHMENT1, etc
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, framebuffer.colorBuffer[i], 0);
		}
		//Explicitly tell OpenGL which color attachments we will draw to
		const GLenum drawBuffers[3] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
		};
		glDrawBuffers(3, drawBuffers);

		//TODO: Add texture2D depth buffer
		glGenTextures(1, &framebuffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer.depthBuffer, 0);
		
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); 
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("G-Buffer incomplete: %d", status);
		}
		//Clean up global state
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return framebuffer;
	}