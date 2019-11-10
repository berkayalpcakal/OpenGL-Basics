#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <string.h>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"


const GLint  WIDTH = 800, HEIGHT = 600;
const float toRadians = glm::pi<float>() / 180.0f;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Window* mainWindow;
Camera* camera;
GLfloat deltaTime = 0.0f;
GLfloat lastTime  = 0.0f;

Texture* brickTexture;
Texture* waterTexture;

bool  direction 	= true;
bool  sizeDirection = true;
float triOffset 	= 0.0f;
float triMaxOffset 	= 0.5f;
float triMoveIncr 	= 0.005f;
float currentAngle 	= 0.0f;
float currentSize 	= 0.5f;
float maxSize 		= 0.8f;
float minSize 		= 0.2f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Pixel Shader
static const char* fShader = "Shaders/shader.fragt";

void CreateObjects()
{
    unsigned int indices[] = { 0, 3, 1,
							   1, 3, 2,
							   3, 2, 0,
							   0, 1, 2  };

	GLfloat vertices[] = 
	{ 
		// x     y     z        u     v 
		-1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 
		0.0f, -1.0f,  1.0f,	   0.5f, 0.0f,
		1.0f, -1.0f,  0.0f,    1.0f, 0.0f,
		0.0f,  1.0f,  0.0f,    0.5f, 1.0f 
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(obj1);	


	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(obj2);		

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow =  new Window(WIDTH,HEIGHT, "");
	mainWindow->initialise();

	CreateObjects();
	CreateShaders();

	camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 5.0f, 20.0f);

	brickTexture = new Texture("Textures/brick.png");
	brickTexture->LoadTexture();

	waterTexture = new Texture("Textures/dirt.png");
	waterTexture->LoadTexture();

	// Projection Matrix
	GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow->getBufferWidth() / (GLfloat)mainWindow->getBufferHeight(), 0.1f, 100.0f);

	// Loop until it is closed
	while (!mainWindow->getShoudlClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;	
		lastTime = now;

		// Get and handle user inputs
		glfwPollEvents();

		camera->keyControl(mainWindow->getKeys(), deltaTime);
		camera->mouseControl(mainWindow->getMouseDeltaX(), mainWindow->getMouseDeltaY(), deltaTime);

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		shaderList[0].UseShader();
		uniformModel 		= shaderList[0].GetModelLocation();
		uniformProjection 	= shaderList[0].GetProjectionLocation();
		uniformView 		= shaderList[0].GetViewLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 	  1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(currentSize, currentSize, currentSize));
		model = glm::rotate(model, currentAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 	  1, GL_FALSE, glm::value_ptr(model));
		brickTexture->UseTexture();
		meshList[0]->RenderMesh();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(currentSize, currentSize, currentSize));
		model = glm::rotate(model, currentAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 	  1, GL_FALSE, glm::value_ptr(model));
		waterTexture->UseTexture();
		meshList[1]->RenderMesh();		

		glUseProgram(0);
		mainWindow->swapBuffers();
	}

	return 0;
}