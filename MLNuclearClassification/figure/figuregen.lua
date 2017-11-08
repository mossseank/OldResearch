--
-- This file is licensed under the MIT license, the text of which can be found in the README file in
--     this repository, or online at <https://opensource.org/licenses/MIT>.
-- Copyright (c) 2017 Sean Moss
--


workspace "figuregen"
	language "C++"
	location "BUILD"
	configurations { "Release" }
	platforms { "x86" }
	targetdir "OUT"
	runtime "Release"
	symbols "Off"
	optimize "Speed"


project "figuregen"
	kind "ConsoleApp"
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	flags { "MultiProcessorCompile", "C++11" }
	includedirs { "src", "lib" }
	architecture "x86"
	libdirs { "lib" }
	filter { "system:windows" }
		links { "glfw3win", "glew32win", "opengl32" }
		defines { "GLEW_STATIC", "_CRT_SECURE_NO_WARNINGS" }
	filter { "system:linux" }
		links { "glfw", "GL" }
	filter { "system:macosx" }
		links { "glfw", "OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreVideo.framework" }
	filter {}