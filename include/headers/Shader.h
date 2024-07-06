#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"


std::string get_file_contents(const char* filename);

class Shader
{
	public:
		GLuint ID;
		Shader() { }
		Shader  &Use();
		Shader(const char* vertexFile, const char* fragmentFile);

		void Activate();
		void Delete();

		    void    SetFloat    (const char *name, float value, bool useShader = false);
		    void    SetInteger  (const char *name, int value, bool useShader = false);
		    void    SetVector2f (const char *name, float x, float y, bool useShader = false);
		    void    SetVector2f (const char *name, const glm::vec2 &value, bool useShader = false);
		    void    SetVector3f (const char *name, float x, float y, float z, bool useShader = false);
		    void    SetVector3f (const char *name, const glm::vec3 &value, bool useShader = false);
		    void    SetVector4f (const char *name, float x, float y, float z, float w, bool useShader = false);
		    void    SetVector4f (const char *name, const glm::vec4 &value, bool useShader = false);
		    void    SetMatrix4  (const char *name, const glm::mat4 &matrix, bool useShader = false);

	private:
		void compileErrors(unsigned int shader, const char* type);
};


#endif