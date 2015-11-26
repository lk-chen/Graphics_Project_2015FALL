#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

extern GLFWwindow* window;
extern const int winWidth;
extern const int winHeight;
extern glm::vec3 eye;
extern glm::vec3 up;

void initDisp();

void displayLoop();

void renewProjection();

void left(float degrees, glm::vec3& eye, glm::vec3& up);

void upside(float degrees, glm::vec3& eye, glm::vec3& up);

void rebindTex();
