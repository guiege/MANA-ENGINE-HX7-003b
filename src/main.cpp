#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <dwmapi.h>

#include <iostream>
#include <math.h>
#include <thread>

#include <glad/glad.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ggponet.h"

// Graphics Includes
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

//fmod Includes
// #include "fmod.h"
// #include "fmod_common.h"

//Engine Includes
#include "ResourceManager.h"
#include "GameState.h"
#include "IntroState.h"
#include "TestState.h"
#include "BatchState.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

const unsigned int width = 1920;
const unsigned int height = 1080;

GameState* gCurrentState = NULL;
GameState* gNextState = NULL;

GLFWwindow* window;

GGPOSession *ggpo = NULL;

void setNextState( GameState* newState )
{
    // //If the user doesn't want to exit
    // if( gNextState != ExitState::get() )
    // {
    //     //Set the next state
    //     gNextState = newState;
    // }
    gNextState = newState;
}

void changeState()
{
    //If the state needs to be changed
    if( gNextState != NULL )
    {
        gCurrentState->exit();
        ResourceManager::Clear();

        gNextState->doneLoading = false;
        gNextState->enter();

        std::thread loadingThread(ResourceManager::UploadToRAM);
        loadingThread.detach();
        
        //Change the current state ID
        gCurrentState = gNextState;
        gCurrentState->tick = 0;
        gNextState = NULL;
    }
}


int main()
{

    glfwInit(); //Initialize GLFW

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Tell GLFW what version of OpenGL we are using(3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Tell GLFW we are using the core profile
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    window = glfwCreateWindow(width, height, "MANA ENGINE HX7-003b", NULL, NULL); //Create a GLFWwindow object of 800 x 800 with the name "OpenGL"
    
    if(window == NULL) //Error check if the window fails to create
    {
       
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); //Introduce the window into the current context

    gladLoadGL(); //Load GLAD so it configures OpenGL

    HWND hwnd = glfwGetWin32Window(window);

    BOOL use_immersive_dark_mode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &use_immersive_dark_mode, sizeof(use_immersive_dark_mode));

    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    GLFWimage icons[1];
    icons[0].pixels = stbi_load("res/textures/icon.png", &icons[0].width, &icons[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, icons);
    stbi_image_free(icons[0].pixels);


    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glViewport(0, 0, width, height); //Specify the viewport of OpenGL in the window from origin corner to bottom right corner

    // glEnable(GL_DEPTH_TEST); ENABLE FOR 3D STUFFs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double crntTime = 0.0;
    double prevTime = 0.0;
    double dt = prevTime;
    double accumulator = 0;
    unsigned int counter = 0;
    unsigned int tick = 0;

    bool show_demo_window = true;

    glfwSwapInterval(0);

    Shader loadingShader("res/shaders/default.vert", "res/shaders/default.frag");
    Shader textShader("res/shaders/text.vert", "res/shaders/text.frag");
    glm::mat4 proj = glm::ortho(0.0f, (float)(width), (float)(height), 0.0f, -1.0f, 1.0f);
    loadingShader.Use().SetInteger("image", 0);
    loadingShader.SetMatrix4("projection", proj);
    Renderer loadingRenderer(loadingShader);
    TextRenderer loadingText(width, height, textShader);
    loadingText.Load("res/fonts/FOTNewRodin Pro B.otf", 24);

    gCurrentState = TestState::get();
    gCurrentState->enter();
    std::thread loadingThread(ResourceManager::UploadToRAM);
    loadingThread.detach();

    //FMOD STUFF

    // FMOD_SYSTEM* system;
    // FMOD_RESULT result=FMOD_System_Create(&system, FMOD_VERSION);

    // FMOD_System_Set3DSettings(system, 1.0,1.0f,1.0f);

    // result=FMOD_System_Init(system, 512,FMOD_INIT_NORMAL | FMOD_INIT_CHANNEL_LOWPASS,nullptr);

    // FMOD_SOUND* sound;
    // result=FMOD_System_CreateSound(system, "testing.mp3",FMOD_DEFAULT,nullptr,&sound);

    // FMOD_CHANNEL*channel;
    // result=FMOD_System_PlaySound(system, sound,nullptr,false,&channel);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while(!glfwWindowShouldClose(window)) //Main while loop
    {
        glfwPollEvents();

        crntTime = glfwGetTime();
        dt = crntTime - prevTime;
        prevTime = crntTime;
        accumulator += dt;
        // counter++;

        // if(dt >= 1.0 / 30.0)
        // {
        //     std::string FPS = std::to_string((1.0/ dt) * counter);
        //     std::string ms = std::to_string((dt / counter) * 1000);
        //     std::string newTitle = "guiengine 0 - " + FPS + "FPS / " + ms + "ms";
        //     glfwSetWindowTitle(window, newTitle.c_str());
        //     prevTime = crntTime;
        //     counter = 0;
        // }

        double cposx, cposy;
        glfwGetCursorPos(window, &cposx, &cposy);

        gCurrentState->mouseX = cposx;
        gCurrentState->mouseY = cposy;

        if(abs(dt - 1.0/120.0) < .0002){
            dt = 1.0/120.0;
        }
        if(abs(dt - 1.0/60.0) < .0002){
            dt = 1.0/60.0;
        }
        if(abs(dt - 1.0/30.0) < .0002){
            dt = 1.0/30.0;
        }

        while(accumulator >= 1.0 / 60.0){
            if(gCurrentState->doneLoading)
                gCurrentState->tick++;
            gCurrentState->update(dt);
            accumulator -= 1.0 / 60.0;
            tick++;
        }

        if(ResourceManager::doneLoading && gCurrentState->doneLoading == false){
            std::cout << "DONE LOADING ASSETS" << std::endl;
            ResourceManager::UploadToGPU();
            gCurrentState->doneLoading = true;
        }

        // FMOD_Channel_SetLowPassGain(channel, (xpos/width));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        if(gCurrentState->doneLoading){
            //Specify the color of the background
            // glClearColor(48/(float)255, 56/(float)255, 65/(float)255, 1.0f);
            glClearColor(38/(float)255, 38/(float)255, 38/(float)255, 1.0f);

            // Clean the back buffer and assign the new color to it
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gCurrentState->render();
        }
        else{
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            loadingRenderer.DrawRect(glm::vec2(840, 520), glm::vec2(240 * (ResourceManager::percentLoading), 100), 0, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            // loadingText.Load("res/fonts/FOTNewRodin Pro B.otf", 24);
            loadingText.RenderText("Loading...", 840, 640, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    //Delete all the objects we've created
    ResourceManager::Clear();

    gCurrentState->exit();
    // delete gCurrentState;
    // delete gNextState;

    // FMOD_System_Release(system);
    // channel->stop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window); //Delete window before ending the program
    glfwTerminate(); //Terminate GLFW before ending the program
    return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
            gCurrentState->leftMouse = true;
        else if(action == GLFW_RELEASE)
            gCurrentState->leftMouse = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if(action == GLFW_PRESS)
            gCurrentState->rightMouse = true;
        else if(action == GLFW_RELEASE)
            gCurrentState->rightMouse = false;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if(action == GLFW_PRESS)
            gCurrentState->middleMouse = true;
        else if(action == GLFW_RELEASE)
            gCurrentState->middleMouse = false;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(IntroState::get());
        changeState();
    }
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(TestState::get());
        changeState();
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(BatchState::get());
        changeState();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key >= 0 && key < 1024)
    {

        if (action == GLFW_PRESS){
            gCurrentState->Keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
            gCurrentState->Keys[key] = false;
    }
}
