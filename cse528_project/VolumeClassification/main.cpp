#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "keyboard.h"
#include "display.h"
#include "loadData.h"

void init()
{
	sx = sy = sz = 1;
	coloredVolume.resize(4);
	curDataIdx = 0;
	curResLevel = 0;

	colorMap = loadColorMap(0);
	coloredVolume = loadVolume(curDataIdx, curResLevel);

	printHelp();

	initDisp();
}

int main() {
	// GLFW init
	if (!glfwInit()) {
		fprintf(stderr, "GLFW failed to initialize.");
		getchar();
		return 1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(winWidth, winHeight, "CSE528 Linkun CHEN", NULL, NULL);

	if (!window) {
		fprintf(stderr, "Window fail to create.");
		glfwTerminate();
		getchar();
		return 1;
	}

	glfwMakeContextCurrent(window);

	// glew init
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "GLEW failed to initialize.");
		glfwTerminate();
		getchar();
		return 1;
	}

	// my own init
	init();

	displayLoop();

	std::cout << "Success.\n";
}


