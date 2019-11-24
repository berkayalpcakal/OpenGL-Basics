#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
    public:
        Shader();

        void CreateFromString(const char* vertexCode, const char* fragmentCode);
        void CreateFromFiles(const char* vertexShaderLocation, const char* fragmentShaderLocation);
        std::string ReadFile(const char* fileLocation);
        GLuint GetProjectionLocation();
        GLuint GetModelLocation();
        GLuint GetViewLocation();
        GLuint GetAmbientIntensityLocation();
        GLuint GetAmbientColorLocation();
        GLuint GetDiffuseIntensityLocation();
        GLuint GetDirectionLocation();
        GLuint GetSpecularIntensityLocation();
        GLuint GetShininessLocation();
        GLuint GetEyePositionLocation();
        void UseShader();
        void ClearShader();

        ~Shader();

    private:
        GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
        uniformAmbientIntensity, uniformAmbientColor, uniformDiffuseIntensity, uniformDirection,
        uniformSpecularIntensity, uniformShininess;

        void CompileShader(const char* vertexCode, const char* fragmentCode);
        void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

};