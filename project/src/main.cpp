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
#include "Light.h"
#include "Material.h"

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

Light* mainLight;

Material* shinyMaterial;
Material* dullMaterial;

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

// Texture Locations
char* brickTextureLocation = "Textures/brick.png";
char* dirtTextureLocation  = "Textures/dirt.png";

void calculateAverageNormals(unsigned int* indices,  unsigned int indecisCount, GLfloat* vertices, 
						unsigned int verticesCount, unsigned int vLength,   unsigned int normalOffset)
{
	for (size_t i = 0; i < indecisCount; i += 3)
	{
		unsigned int v0idx = indices[i]   * vLength;
		unsigned int v1idx = indices[i+1] * vLength;
		unsigned int v2idx = indices[i+2] * vLength;

		glm::vec3 edge1(vertices[v1idx] - vertices[v0idx], vertices[v1idx + 1] - vertices[v0idx + 1], vertices[v1idx + 2] - vertices[v0idx + 2]);
		glm::vec3 edge2(vertices[v2idx] - vertices[v0idx], vertices[v2idx + 1] - vertices[v0idx + 1], vertices[v2idx + 2] - vertices[v0idx + 2]);
		glm::vec3 normal = glm::cross(edge1, edge2);
		normal = glm::normalize(normal);

		v0idx += normalOffset; v1idx += normalOffset; v2idx += normalOffset;
		vertices[v0idx] += normal.x; vertices[v0idx + 1] += normal.y; vertices[v0idx + 2] += normal.z;
		vertices[v1idx] += normal.x; vertices[v1idx + 1] += normal.y; vertices[v1idx + 2] += normal.z;
		vertices[v2idx] += normal.x; vertices[v2idx + 1] += normal.y; vertices[v2idx + 2] += normal.z;
	}

	for (size_t i = 0; i < verticesCount / vLength; i++)
	{
		unsigned int normalIDX = i * vLength + normalOffset;
		glm::vec3 vec(vertices[normalIDX], vertices[normalIDX + 1], vertices[normalIDX + 2]);
		vec = glm::normalize(vec);
		vertices[normalIDX] = vec.x; vertices[normalIDX + 1] = vec.y; vertices[normalIDX + 2] = vec.z;
	}	
}

void CreateObjects()
{
    unsigned int indices[] = { 0, 3, 1,
							   1, 3, 2,
							   2, 3, 0,
							   0, 1, 2 };

	GLfloat vertices[] = 
	{ 
		// x     y     z        u     v 	  nx    ny    nz
		-1.0f, -1.0f, 0.0f,    0.0f, 0.0f,    0.0f, 0.0f, 0.0f,
		0.0f, -1.0f,  1.0f,	   0.5f, 0.0f,    0.0f, 0.0f, 0.0f,
		1.0f, -1.0f,  0.0f,    1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
		0.0f,  1.0f,  0.0f,    0.5f, 1.0f,    0.0f, 0.0f, 0.0f 
	};

	calculateAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);	


	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
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
	// Initialize window
	mainWindow =  new Window(WIDTH,HEIGHT, "");
	mainWindow->initialise();

	// GL OPTIONS
	glEnable(GL_DEPTH_TEST);	

	// Create Objects
	CreateObjects();

	// Create Shaders
	CreateShaders();

	// Create Textures
	brickTexture = new Texture(brickTextureLocation);
	brickTexture->LoadTexture();

	waterTexture = new Texture(dirtTextureLocation);
	waterTexture->LoadTexture();

	// Camera
	camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 20.0f);

	// Light
	mainLight = new Light(    1.0f, 1.0f, 1.0f,   0.3f, 
					glm::vec3(2.0f, -1.0, -2.0f), 0.7f);

	// Materials
	shinyMaterial = new Material(1.0f, 32);
	dullMaterial  = new Material(1.0f, 4);

	// Projection Matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow->getBufferWidth() / (GLfloat)mainWindow->getBufferHeight(), 0.1f, 100.0f);

	
	glm::vec3 cameraPosition;

	GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0, uniformEyePosition = 0,
		   uniformAmbientColor = 0, uniformAmbientIntensity = 0, uniformDirection = 0, uniformDiffuseIntensity = 0,
		   uniformSpecularIntensity = 0, uniformShininess = 0;
	
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
		cameraPosition = camera->GetCameraPosition();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		shaderList[0].UseShader();
		uniformModel 			 = shaderList[0].GetModelLocation();
		uniformProjection 		 = shaderList[0].GetProjectionLocation();
		uniformView 			 = shaderList[0].GetViewLocation();
		uniformAmbientColor 	 = shaderList[0].GetAmbientColorLocation();
		uniformAmbientIntensity  = shaderList[0].GetAmbientIntensityLocation();
		uniformDirection 		 = shaderList[0].GetDirectionLocation();
		uniformDiffuseIntensity  = shaderList[0].GetDiffuseIntensityLocation();
		uniformEyePosition       = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess		 = shaderList[0].GetShininessLocation();
		
		mainLight->UseLight(uniformAmbientIntensity, uniformAmbientColor, uniformDiffuseIntensity, uniformDirection);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 	  1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));
		glUniform3f(uniformEyePosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.5f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial->UseMaterial(uniformSpecularIntensity, uniformShininess);
		brickTexture->UseTexture();
		meshList[0]->RenderMesh();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.5f, -0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dullMaterial->UseMaterial(uniformSpecularIntensity, uniformShininess);
		waterTexture->UseTexture();
		meshList[1]->RenderMesh();		

		glUseProgram(0);
		mainWindow->swapBuffers();
	}

	return 0;
}