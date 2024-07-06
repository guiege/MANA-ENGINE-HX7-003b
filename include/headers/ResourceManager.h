#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "stb/stb_image.h"

#include "Texture.h"
#include "Shader.h"
#include "Util.h"

class ResourceManager
{
public:
    // resource storage
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture> Textures;
    static std::vector<TextureData> TextureQueue; 
    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name);
    // retrieves a stored shader
    static Shader&    GetShader(std::string name);
    static void UploadToGPU();
    static void UploadToRAM();
    
    static void LoadTexture(const char *file, bool alpha, std::string name);
    // retrieves a stored texture
    static Texture& GetTexture(std::string name);
    // properly de-allocates all loaded resources
    static void      Clear();

    static bool doneLoading;
    static float percentLoading;


private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // loads a single texture from file
};

#endif