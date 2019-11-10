#include "Window.h"

Window::Window() : windowWidth(800), windowHeight(600), windowTitle("Test Window")
{
	for (auto& key : keys)
	{
		key = 0;
	}

	isMouseInitialized = false;
}

Window::Window(GLint width, GLint height, const char* title) : windowWidth(width),
															   windowHeight(height),
															   windowTitle(title)
{
	for (auto& key : keys)
	{
		key = 0;
	}

	isMouseInitialized = false;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getMouseDeltaX()
{
	GLfloat deltaX = mouseXDelta;
	mouseXDelta = 0.0f;
	return deltaX;
}

GLfloat Window::getMouseDeltaY()
{
	GLfloat deltaY = mouseYDelta;
	mouseYDelta = 0.0f;
	return deltaY;
}

int Window::initialise()
{
	// Initialise GLFW 
	if (!glfwInit())
	{
		std::cout << "GLFW Initialisation has failed!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Setup GLFW Window Properties
	// OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile means that there will be no backward compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(windowWidth, windowHeight, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		std::cout << "GLFW Window Creation has failed!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Get Framebuffer size information
	glfwGetFramebufferSize(mainWindow, &windowBufferWidth, &windowBufferHeight);

	// Set context for GLFW to use
	glfwMakeContextCurrent(mainWindow);

	// Create callbacks to handle keyboard and mouse events
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Initialisation has failed!" << std::endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Set Viewport Size
	glViewport(0, 0, windowBufferWidth, windowBufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	Window* windowPtrGLFW = static_cast<Window*>(glfwGetWindowUserPointer(window));
	
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			windowPtrGLFW->keys[key] = true;
			// std::cout << "Key " << key << " is pressed." << std::endl;
		}
		else if (action == GLFW_RELEASE)
		{
			windowPtrGLFW->keys[key] = false;
			// std::cout << "Key " << key << " is released." << std::endl;
		}
	}
	
}

void Window::handleMouse(GLFWwindow* window, double xPosition, double yPosition)
{
	Window* windowPtrGLFW = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!windowPtrGLFW->isMouseInitialized)
	{
		windowPtrGLFW->mouseX = xPosition;
		windowPtrGLFW->mouseY = yPosition;
		windowPtrGLFW->isMouseInitialized = true;
	}

	windowPtrGLFW->mouseXDelta = xPosition - windowPtrGLFW->mouseX;
	windowPtrGLFW->mouseYDelta = windowPtrGLFW->mouseY - yPosition;

	windowPtrGLFW->mouseX = xPosition;
	windowPtrGLFW->mouseY = yPosition;

	// std::cout << "X : " << windowPtrGLFW->mouseX << " Y: " << windowPtrGLFW->mouseY << std::endl;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}