#include "ResourceManager.h"

// Instantiate static variables
std::map<std::string, Texture>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::vector<TextureData>                 ResourceManager::TextureQueue; 

bool ResourceManager::doneLoading = false;
float ResourceManager::percentLoading = 0.0f;


Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name)
{
    Shaders[name] = Shader(vShaderFile, fShaderFile);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

void ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    TextureData data;

    data.name = name;
    data.alpha = alpha;
    data.file = file;

    TextureQueue.push_back(data);
}

void ResourceManager::LoadTexture(const char *file, bool alpha, unsigned int FILTER_MIN, unsigned int FILTER_MAX, std::string name)
{
    TextureData data;

    data.name = name;
    data.alpha = alpha;
    data.file = file;
    data.Filter_Min = FILTER_MIN;
    data.Filter_Max = FILTER_MAX;

    TextureQueue.push_back(data);
}

Texture& ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    doneLoading = false;
    percentLoading = 0.0f;
    // (properly) delete all shaders
    for (auto iter : Shaders)
        iter.second.Delete();
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);

    TextureQueue.clear();
    Textures.clear();
}

void ResourceManager::UploadToRAM()
{
    doneLoading = false;

    for(int i = 0; i <  TextureQueue.size(); i++)
    {
        TextureQueue[i].data = stbi_load(TextureQueue[i].file.c_str(), &TextureQueue[i].width, &TextureQueue[i].height, &TextureQueue[i].nrChannels, 0);
        ResourceManager::percentLoading = static_cast<float>(i) / TextureQueue.size();
    }

    doneLoading = true;

}

void ResourceManager::UploadToGPU()
{
    for(int i = 0; i < TextureQueue.size(); i++)
    {
        Texture texture;
        texture.Filter_Min = TextureQueue[i].Filter_Min;
        texture.Filter_Max = TextureQueue[i].Filter_Max;
        if(TextureQueue[i].alpha)
        {
            texture.Internal_Format = GL_RGBA;
            texture.Image_Format = GL_RGBA;
        }
        texture.Generate(TextureQueue[i].width, TextureQueue[i].height, TextureQueue[i].data);
        stbi_image_free(TextureQueue[i].data);
        Textures[TextureQueue[i].name] = texture;
    }
    TextureQueue.clear();
}