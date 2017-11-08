/*
 * This file is licensed under the MIT license, the text of which can be found in the README file in
 *     this repository, or online at <https://opensource.org/licenses/MIT>.
 * Copyright(c) 2017 Sean Moss
 */

#include <iostream>
#include "graphics.hpp"


GLFWwindow *g_windowPtr = nullptr;
grph::RenderTarget g_renderTarget {};

void _glfwErrorCallback(int err, const char *errstr)
{
	std::cerr << "ERROR: Caught GLFW error (" << err << "): '" << errstr << "'." << std::endl;
}


namespace grph
{

// ====================================================================================================================
GLFWwindow* createRenderContext()
{
	// Initialize glfw
	glfwSetErrorCallback(_glfwErrorCallback);
	if (glfwInit() != GLFW_TRUE)
		return nullptr;

	// Setup window hints
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE); // Important
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	//glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	// Create window
	g_windowPtr = glfwCreateWindow(1, 1, "figuregen offscreen renderer", nullptr, nullptr);
	if (!g_windowPtr) {
		std::cerr << "ERROR: Could not create offscreen render window." << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(g_windowPtr);

	// Initialize glew
	GLenum error;
	if ((error = glewInit()) != GLEW_OK) {
		const char * const errstr = reinterpret_cast<const char * const>(glewGetErrorString(error));
		std::cerr << "ERROR: Could not initialize GLEW. Reason: '" << errstr << "'." << std::endl;
		glfwTerminate();
		return nullptr;
	}

	return g_windowPtr;
}

// ====================================================================================================================
void destroyRenderContext()
{
	if (g_windowPtr)
		glfwDestroyWindow(g_windowPtr);
	g_windowPtr = nullptr;

	glfwTerminate();
}


// ====================================================================================================================
RenderTarget* createRenderTarget(unsigned int fbres)
{
	// Generate the framebuffer object
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	if (!fbo) {
		std::cerr << "ERROR: Could not create render target framebuffer (" << glGetError() << ")." << std::endl;
		return nullptr;
	}

	// Create the three output textures
	unsigned int textures[3];
	glGenTextures(3, textures);
	if (!textures[0] || !textures[1] || !textures[2]) {
		std::cerr << "ERROR: Could not create the render target texture attachments (" << glGetError() << ")." 
				<< std::endl;
		glDeleteFramebuffers(1, &fbo);
		return nullptr;
	}

	// Allocate texture memory
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, fbres, fbres, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, fbres, fbres, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, fbres, fbres, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLenum error;
	if ((error = glGetError()) != GL_NONE) {
		std::cerr << "ERROR: Unable to allocate render target texture memory (" << error << ")." << std::endl;
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(3, textures);
		return nullptr;
	}

	// Attach the textures as framebuffer color attachments
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures[2], 0);
	if ((error = glGetError()) != GL_NONE) {
		std::cerr << "ERROR: Unable to attach render target textures (" << error << ")." << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(3, textures);
		return nullptr;
	}

	// Validate the framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR: Render target framebuffer is not complete (" << glGetError() << ")." << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(3, textures);
		return nullptr;
	}

	// Populate structure and return
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	g_renderTarget.fbhandle = fbo;
	g_renderTarget.photoTexHandle = textures[0];
	g_renderTarget.rsTexHandle = textures[1];
	g_renderTarget.bsTexHandle = textures[2];
	g_renderTarget.resolution = fbres;
	return &g_renderTarget;
}

// ====================================================================================================================
void destroyRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &(g_renderTarget.fbhandle));

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &(g_renderTarget.photoTexHandle));
	glDeleteTextures(1, &(g_renderTarget.rsTexHandle));
	glDeleteTextures(1, &(g_renderTarget.bsTexHandle));
}

}