// MANA FIGHTING GAME ENGINE by guiege(Christian Schmitz)
// FOR SHUKLA SHORES OFFICIAL USE ONLY

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
#include "Scene.h"
#include "IntroState.h"
#include "HitboxEditorState.h"
#include "BatchState.h"
#include "SplashState.h"
#include "OnlineLobby.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

Scene* gCurrentState = NULL;
Scene* gNextState = NULL;

GLFWwindow* window;
int windowWidth = 1920;
int windowHeight = 1080;
int monitorWidth, monitorHeight;
bool isFullscreen = false;

unsigned int tick = 0;

void updateViewport()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (isFullscreen)
    {
        // Calculate the viewport to maintain 1080p aspect ratio
        float targetAspectRatio = (float)windowWidth / windowHeight;
        int viewportWidth = width;
        int viewportHeight = (int)(width / targetAspectRatio);
        
        if (viewportHeight > height)
        {
            viewportHeight = height;
            viewportWidth = (int)(height * targetAspectRatio);
        }

        int vpX = (width - viewportWidth) / 2;
        int vpY = (height - viewportHeight) / 2;

        glViewport(vpX, vpY, viewportWidth, viewportHeight);
    }
    else
    {
        glViewport(0, 0, windowWidth, windowHeight);
    }
}

void toggleFullscreen()
{
    if (!isFullscreen)
    {
        // Get the primary monitor
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        // Store the monitor's resolution
        monitorWidth = mode->width;
        monitorHeight = mode->height;

        // Create a fullscreen window
        glfwSetWindowMonitor(window, primaryMonitor, 0, 0, monitorWidth, monitorHeight, mode->refreshRate);
        
        isFullscreen = true;
    }
    else
    {
        // Calculate position to center the window
        int xpos = (monitorWidth - windowWidth) / 2;
        int ypos = (monitorHeight - windowHeight) / 2;

        // Restore windowed mode
        glfwSetWindowMonitor(window, nullptr, xpos, ypos, windowWidth, windowHeight, 0);
        
        isFullscreen = false;
    }

    // Update viewport
    updateViewport();
}

void setNextState( Scene* newState )
{
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
        gNextState->enter(gCurrentState->args);

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
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    window = glfwCreateWindow(windowWidth, windowHeight, "MANA ENGINE HX7-003b", NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    gladLoadGL();

    updateViewport();

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
    glViewport(0, 0, windowWidth, windowHeight); //Specify the viewport of OpenGL in the window from origin corner to bottom right corner

    // glEnable(GL_DEPTH_TEST); ENABLE FOR 3D STUFFs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool show_demo_window = false;

    glfwSwapInterval(0);

    double update_rate = 60; //Lock the tickrate to 60 frames per second
    int update_multiplicity = 1;
    bool unlock_framerate = false;

    //compute how many ticks per second one update should be
    int64_t clocks_per_second = glfwGetTimerFrequency();
    double fixed_deltatime = 1.0 / (update_rate * 10);
    int64_t desired_frametime = clocks_per_second / (update_rate * 10);

    //these are to snap deltaTime to vsync values if it's close enough
    int64_t vsync_maxerror = clocks_per_second * .0002;

    //get the refresh rate of the display (you should detect which display the window is on in production)
    int display_framerate = 60;
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    display_framerate = mode->refreshRate;
    int64_t snap_hz = display_framerate;
    if(snap_hz <= 0) snap_hz = 60;

    //these are to snap deltaTime to vsync values if it's close enough
    int64_t snap_frequencies[8] = {};
    for(int i = 0; i<8;i++){
        snap_frequencies[i] = (clocks_per_second / snap_hz) * (i+1);
    }

    const int time_history_count = 4;
    int64_t time_averager[time_history_count] = {desired_frametime, desired_frametime, desired_frametime, desired_frametime};
    int64_t averager_residual = 0;

    bool running = true;
    bool resync = true;
    int64_t prev_frame_time = static_cast<int64_t>(glfwGetTime() * 1000000.0);
    int64_t frame_accumulator = 0;

    Shader loadingShader("res/shaders/default.vert", "res/shaders/default.frag");
    Shader textShader("res/shaders/text.vert", "res/shaders/text.frag");
    glm::mat4 proj = glm::ortho(0.0f, (float)(windowWidth), (float)(windowHeight), 0.0f, -1.0f, 1.0f);
    loadingShader.Use().SetInteger("image", 0);
    loadingShader.SetMatrix4("projection", proj);
    Renderer loadingRenderer(loadingShader);
    TextRenderer loadingText(windowWidth, windowHeight, textShader);
    loadingText.Load("res/fonts/FOTNewRodin Pro B.otf", 24);

    gCurrentState = SplashState::get();
    gCurrentState->enter(gCurrentState->args);
    std::thread loadingThread(ResourceManager::UploadToRAM);
    loadingThread.detach();

    //FMOD STUFF

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while(!glfwWindowShouldClose(window)) //Main while loop. Huge thanks to TylerGlaiel
    {
        glfwPollEvents();
        double cposx, cposy;
        glfwGetCursorPos(window, &cposx, &cposy);

        gCurrentState->mouseX = cposx;
        gCurrentState->mouseY = cposy;
        
        int64_t current_frame_time = static_cast<int64_t>(glfwGetTime() * 1000000.0);
        int64_t delta_time = current_frame_time - prev_frame_time;
        int delta_ms = static_cast<int>(delta_time / 1000);
        prev_frame_time = current_frame_time;

        if(delta_time > desired_frametime*8){
            delta_time = desired_frametime;
        }
        if(delta_time < 0){
            delta_time = 0;
        }

        //vsync time snapping
        for(int64_t snap : snap_frequencies){
            if(std::abs(delta_time - snap) < vsync_maxerror){
                delta_time = snap;
                break;
            }
        }

        //delta time averaging
        for(int i = 0; i<time_history_count-1; i++){
            time_averager[i] = time_averager[i+1];
        }
        time_averager[time_history_count-1] = delta_time;
        int64_t averager_sum = 0;
        for(int i = 0; i<time_history_count; i++){
            averager_sum += time_averager[i];
        }
        delta_time = averager_sum / time_history_count;

        averager_residual += averager_sum % time_history_count;
        delta_time += averager_residual / time_history_count;
        averager_residual %= time_history_count;

        //add to the accumulator
        frame_accumulator += delta_time;

        //spiral of death protection
        if(frame_accumulator > desired_frametime*8){
            resync = true;
        }

        //timer resync if required
        if(resync){
            frame_accumulator = 0;
            delta_time = desired_frametime;
            resync = false;
        }


        if(ResourceManager::doneLoading && gCurrentState->doneLoading == false){
            std::cout << "DONE LOADING ASSETS" << std::endl;
            ResourceManager::UploadToGPU();
            gCurrentState->doneLoading = true;
        }

        gCurrentState->network(glm::max(0,delta_ms-1));

        if(unlock_framerate){
            int64_t consumedDeltaTime = delta_time;

            while(frame_accumulator >= desired_frametime){
                if(consumedDeltaTime > desired_frametime){
                    //game.variable_update(fixed_deltatime)
                    consumedDeltaTime -= desired_frametime;
                }
                gCurrentState->update(fixed_deltatime);
                consumedDeltaTime -= desired_frametime;
            }
            frame_accumulator -= desired_frametime;

            //game.variable_update((double)consumedDeltaTime / clocks_per_second);
            //render
        } else { //LOCKED FRAMERATE, NO INTERPOLATION
            while(frame_accumulator >= desired_frametime*update_multiplicity){
                for(int i = 0; i<update_multiplicity; i++){
                    gCurrentState->update(fixed_deltatime);
                    if(gCurrentState->doneLoading){
                        gCurrentState->tick++;
                        if(gCurrentState->desiredState > 0){
                            if(gCurrentState->desiredState == 1){
                                setNextState(IntroState::get());
                                changeState();
                            }
                            if(gCurrentState->desiredState == 2){
                                setNextState(HitboxEditorState::get());
                                changeState();
                            }
                            if(gCurrentState->desiredState == 3){
                                setNextState(BatchState::get());
                                changeState();
                            }
                            if(gCurrentState->desiredState == 4){
                                setNextState(OnlineLobby::get());
                                changeState();
                            }
                        }
                    }
                    frame_accumulator -= desired_frametime;
                    tick++;

                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();

                    if(gCurrentState->doneLoading){
                        //Specify the color of the background
                        // glClearColor(48/(float)255, 56/(float)255, 65/(float)255, 1.0f);
                        glClearColor(55/(float)255, 55/(float)255, 55/(float)255, 1.0f);

                        // Clean the back buffer and assign the new color to it
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        gCurrentState->render();
                    }
                    else{
                        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        loadingRenderer.DrawRect(glm::vec2(840, 520), glm::vec2(240 * (ResourceManager::percentLoading), 100), 0, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                        loadingText.RenderText("Loading...", 840, 640, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                    }

                    if (show_demo_window)
                        ImGui::ShowDemoWindow(&show_demo_window);

                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                    glfwSwapBuffers(window);
                }
            }

        }

        // FMOD_Channel_SetLowPassGain(channel, (xpos/width));

    }

    ResourceManager::Clear();

    gCurrentState->exit();

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT && action == GLFW_PRESS)
    {
        toggleFullscreen();
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(IntroState::get());
        changeState();
    }
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(HitboxEditorState::get());
        changeState();
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(BatchState::get());
        changeState();
    }
    if (key == GLFW_KEY_F4 && action == GLFW_PRESS && gCurrentState->doneLoading){
        setNextState(OnlineLobby::get());
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

    // if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
    //      gCurrentState->update(1.0f);
    //     gCurrentState->tick++;
    // }
}