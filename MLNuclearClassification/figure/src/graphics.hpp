/*
 * This file is licensed under the MIT license, the text of which can be found in the README file in
 *     this repository, or online at <https://opensource.org/licenses/MIT>.
 * Copyright(c) 2017 Sean Moss
 */

#pragma once
#define GRAPHICS_HPP_

#include <GL\glew.h>
#include <glfw3.h>


namespace grph
{

struct RenderTarget
{
	unsigned int fbhandle;
	unsigned int photoTexHandle;
	unsigned int rsTexHandle;
	unsigned int bsTexHandle;
	unsigned int resolution;
};


/* Windowing Controls */
extern GLFWwindow* createRenderContext();
extern void destroyRenderContext();

/* Render Target Controls */
extern RenderTarget* createRenderTarget(unsigned int fbres);
extern void destroyRenderTarget();

} // namespace grph