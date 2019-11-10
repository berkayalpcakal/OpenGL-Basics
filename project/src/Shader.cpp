#include "Shader.h"

Shader::Shader()
{
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
}


void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
    CompileShader(vertexCode, fragmentCode);
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
    shaderID = glCreateProgram();

	if (!shaderID)
	{
		std::cout << "Shader program compilation has failed!" << std::endl;
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	GLint  _result = 0;
	GLchar _eCode[1024] = { 0 };

	glLinkProgram(shaderID);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &_result);
	if (!_result)
	{
		glGetProgramInfoLog(shaderID, sizeof(_eCode), NULL, _eCode);
		std::cout << "Error happened while linking program " << _eCode << std::endl;
		return;
	}

	glValidateProgram(shaderID);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &_result);
	if (!_result)
	{
		glGetProgramInfoLog(shaderID, sizeof(_eCode), NULL, _eCode);
		std::cout << "Error happened while validating program " << _eCode << std::endl;
		return;
	}

	uniformModel 	  = glGetUniformLocation(shaderID, "model");
	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformView       = glGetUniformLocation(shaderID, "view");
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint  theShader = glCreateShader(shaderType);
	const GLchar* c[1];
	c[0] = shaderCode;

	GLint l[1];
	l[0] = strlen(shaderCode);
	glShaderSource(theShader, 1, c, l);
	glCompileShader(theShader);

	GLint  _result = 0;
	GLchar _eCode[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &_result);
	if (!_result)
	{
		glGetShaderInfoLog(theShader, sizeof(_eCode), NULL, _eCode);
		std::cout << "Error happened while compuling " << shaderType << " shader" << _eCode << std::endl;
		return;
	}

	glAttachShader(theProgram, theShader);
}

GLuint Shader::GetProjectionLocation()
{
    return uniformProjection;
}


GLuint Shader::GetModelLocation()
{
    return uniformModel;
}

GLuint Shader::GetViewLocation()
{
	return uniformView;
}


void Shader::UseShader()
{
    glUseProgram(shaderID);
}

void Shader::ClearShader()
{
    if(shaderID != 0)
    {
        glDeleteShader(shaderID);
        shaderID = 0;
    }
    uniformProjection = 0;
    uniformModel = 0;
}

std::string Shader::ReadFile(const char* fileLocation)
{
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if(!fileStream.is_open())
    {
        printf("Failed to read file %s!", fileLocation);
        return "";
    }

    std::string line = "";
    while(!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

void Shader::CreateFromFiles(const char* vertexShaderLocation, const char* fragmentShaderLocation)
{
    std::string vertexString   = ReadFile(vertexShaderLocation);
    std::string fragmentString = ReadFile(fragmentShaderLocation);

    const char* vertexCode   = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    CompileShader(vertexCode, fragmentCode);
}

Shader::~Shader()
{
    ClearShader();
}