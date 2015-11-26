#include "display.h"
#include <vector>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "shaders.h"
#include "keyboard.h"
#include "loadData.h"

GLFWwindow* window;
GLuint shaderProgram;
const int winWidth = 1024;
const int winHeight = 1024;

glm::vec3 eye;
glm::vec3 center;
glm::vec3 up;
glm::mat4 trans;			// transform matrix, should be product of scale and rotate.
glm::mat4 view;				// create from lookat(eye, center, up).
glm::mat4 proj;				// create form perspective
float fovy, aspect, zNear, zFar;

GLfloat verts[] = {
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
	0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f
};
GLuint elements[] = {
	// counter-clock-wise for front
	2,1,0,
	0,3,2,
	7,3,0,
	0,4,7,
	0,1,5,
	5,4,0,
	1,2,6,
	6,5,1,
	2,3,7,
	7,6,2,
	4,5,6,
	6,7,4
};
GLuint volume3DTex, backface2DTex;
GLuint vaoID, vboID, ebo;	// vertex array obj, vertex buffer obj, element buffer obj.
GLuint dpb, frb;			// depth buffer, frame buffer.

void initDisp()
{
	eye = glm::vec3(1.5f, 1.5f, 1.5f);
	center = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 0.0f, 1.0f);
	fovy = glm::radians(45.0f);
	aspect = 8.0f / 6.0f;
	zNear = 1.0f;
	zFar = 10.0f;

	shaderProgram = loadProgram("shaders/shader.vert", "shaders/shader.frag");
	glfwSetKeyCallback(window, key_callback);

	// Set model view projection
	renewProjection();

	// Set up texture
	glGenTextures(1, &volume3DTex);
	glBindTexture(GL_TEXTURE_3D, volume3DTex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_3D);

	// Set 3D volume data
	rebindTex();

	// Generating vertex array object
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// Generate vertex buffer object
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	// Elements buffer
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(elements), elements, GL_STATIC_DRAW);
}

void displayLoop() {
	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "vert_position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), 0);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "vert_texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib,3,GL_FLOAT,GL_FALSE,
		6 * sizeof(float), (void*)(3 * sizeof(float)));

	// enable
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	do
	{
		//glClearColor(0.0f, 0.2f, 0.3f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// change verts
		GLfloat tempVerts[48];
		auto downSampleRate = pow(2, curResLevel);
		for (int i = 0; i < 48; i++)
		{
			tempVerts[i] = verts[i];
		}
		for (int i = 0; i < 8; i++)
		{
			tempVerts[6 * i + 0] = verts[6 * i + 0] / downSampleRate;
			tempVerts[6 * i + 1] = verts[6 * i + 1] / downSampleRate;
			tempVerts[6 * i + 2] = verts[6 * i + 2] / downSampleRate;
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(tempVerts), tempVerts, GL_STATIC_DRAW);

		// Drawing
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));

	glDeleteTextures(1, &volume3DTex);
	glDisableVertexAttribArray(posAttrib);
}

void renewProjection()
{
	GLint uniEye = glGetUniformLocation(shaderProgram, "eye");
	glUniform3fv(uniEye, 1, glm::value_ptr(eye));

	view = glm::lookAt(eye, center, up);
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	proj = glm::perspective(fovy, aspect, zNear, zFar);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

glm::mat3 rotate(const float degrees, const glm::vec3& axis)
{
	glm::mat3 ret;
	glm::mat3 rotateMat;
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	rotateMat = cos(glm::radians(degrees))*glm::mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
	rotateMat += (1 - cos(glm::radians(degrees)))*
		glm::mat3(x*x, x*y, x*z, x*y, y*y, y*z, x*z, y*z, z*z);
	rotateMat += sin(glm::radians(degrees))*
		glm::mat3(0, z, -y, -z, 0, x, y, -x, 0);

	return rotateMat;
}

void left(float degrees, glm::vec3& eye, glm::vec3& up)
{
	glm::mat3 rotateMat = rotate(degrees, glm::normalize(up));
	eye = rotateMat * eye;
	up = rotateMat * up;
}

void upside(float degrees, glm::vec3& eye, glm::vec3& up)
{
	glm::vec3 upAxis = glm::normalize(glm::cross(eye, up));
	glm::mat3 rotateMat = rotate(-degrees, upAxis);
	eye = rotateMat * eye;
	up = rotateMat * up;
}

void rebindTex()
{
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sx, sy, sz, 0, GL_RGBA, GL_FLOAT, (&coloredVolume[0]));
}


