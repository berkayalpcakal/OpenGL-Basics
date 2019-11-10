#pragma once
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight, const char* title);
	
	int initialise();
	GLfloat getMouseDeltaX();
	GLfloat getMouseDeltaY();

	bool* getKeys()         { return keys; }
	void  swapBuffers()     { glfwSwapBuffers(mainWindow); }
	bool  getShoudlClose()  { return glfwWindowShouldClose(mainWindow); }
	GLint getBufferWidth()  { return windowBufferWidth; }
	GLint getBufferHeight() { return windowBufferHeight; }

	~Window();

private:
	GLfloat mouseX, mouseY, mouseXDelta, mouseYDelta;
	GLint windowBufferWidth, windowBufferHeight;
	GLint windowWidth, windowHeight;
	const char* windowTitle;
	bool isMouseInitialized;
	GLFWwindow* mainWindow;
	bool keys[1024];

	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPosition, double yPosition);

	void createCallbacks();
};