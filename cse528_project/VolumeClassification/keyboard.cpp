#include "keyboard.h"
#include "display.h"
#include "loadData.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>

using namespace std;

void changeToDataSet(int idx)
{
	if (idx < numOfDataSet)
	{
		curDataIdx = idx;
		curResLevel = 0;
		coloredVolume = loadVolume(curDataIdx, curResLevel);
		rebindTex();
		std::cout << "Chnage to data set " << idx << '\n';
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE)return;
	switch (key)
	{
	case GLFW_KEY_UP:
		upside(5, eye, up);
		renewProjection();
		break;
	case GLFW_KEY_DOWN:
		upside(-5, eye, up);
		renewProjection();
		break;
	case GLFW_KEY_LEFT:
		left(5, eye, up);
		renewProjection();
		break;
	case GLFW_KEY_RIGHT:
		left(-5, eye, up);
		renewProjection();
		break;
	case GLFW_KEY_PAGE_DOWN:
		curResLevel--;
		curResLevel = curResLevel < 0 ? 0 : curResLevel;
		coloredVolume = loadVolume(curDataIdx, curResLevel);
		rebindTex();
		cout << "Current level " << curResLevel << endl;
		break;
	case GLFW_KEY_PAGE_UP:
		curResLevel++;
		curResLevel = curResLevel > maxResLevel ? maxResLevel : curResLevel;
		coloredVolume = loadVolume(curDataIdx, curResLevel);
		rebindTex();
		cout << "Current level " << curResLevel << endl;
		break;
	case GLFW_KEY_H:
		printHelp();
		break;
	case GLFW_KEY_R:
		renewProjection();
		break;
	case GLFW_KEY_0:
		changeToDataSet(0);
		break;
	case GLFW_KEY_1:
		changeToDataSet(1);
		break;
	case GLFW_KEY_2:
		changeToDataSet(2);
		break;
	case GLFW_KEY_3:
		changeToDataSet(3);
		break;
	case GLFW_KEY_4:
		changeToDataSet(4);
		break;
	case GLFW_KEY_5:
		changeToDataSet(5);
		break;
	case GLFW_KEY_6:
		changeToDataSet(6);
		break;
	case GLFW_KEY_7:
		changeToDataSet(7);
		break;
	case GLFW_KEY_8:
		changeToDataSet(8);
		break;
	case GLFW_KEY_9:
		changeToDataSet(9);
		break;
	case GLFW_KEY_ESCAPE:
		exit(1);
	default:
		break;
	}
}

void printHelp() {
	cout << "\n---------------------------------------------\n"
		<< "press 'h' to print this message again.\n"
		<< "press 'page up' or 'page down' to change the resolution.\n"
		/*<< "press 's' to see sparse PDF result.\n"
		<< "press 'g' to see gaussian result.\n"
		<< "press 'm' to see mean result.\n"*/
		<< "press number to change data.\n"
		<< "press ESC to quit.\n";
}