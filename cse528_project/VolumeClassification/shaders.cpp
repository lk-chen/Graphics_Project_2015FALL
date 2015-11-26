#include "shaders.h"
#include <iostream>
#include <string>
#include <fstream>
#include <GL\glew.h>

using namespace std;

string fileToString(const char* filepath) {
	string fileData;
	ifstream stream(filepath, ios::in);

	if (stream.is_open()) {
		string line = "";

		while (getline(stream, line)) {
			fileData += "\n" + line;
		}

		stream.close();
	}

	return fileData;
}

void GLSLError(const GLint object) {
	GLint length;
	GLchar * log;

	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &length);
	log = new GLchar[length + 1];

	glGetShaderInfoLog(object, length, &length, log);
	cout << "Compile Error, Log Below\n" << log << "\n";

	delete[] log;
	getchar();
	exit(1);
}

GLuint loadShaders(GLenum type, const char* shaderPath) {
	GLuint shader = glCreateShader(type);
	string shaderSource = fileToString(shaderPath);
	const char* rawShaderSource = shaderSource.c_str();
	GLint compiled;

	glShaderSource(shader, 1, &rawShaderSource, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLSLError(shader);
	}

	return shader;
}

GLuint loadProgram(const char* vertShaderPath, const char* fragShaderPath) {
	GLuint program = glCreateProgram();
	GLint linked;
	GLuint vertShader = loadShaders(GL_VERTEX_SHADER, vertShaderPath);
	GLuint fragShader = loadShaders(GL_FRAGMENT_SHADER, fragShaderPath);

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked)
	{
		glUseProgram(program);
	}
	else
	{
		GLSLError(program);
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}
