#include "SplashState.h"

std::string currentLogo = "logo_tm";

	
bool SplashState::exit()
{
	batchRenderer->Delete();
	delete batchRenderer;

	post->Delete();
	delete post;

	return true;
}

bool SplashState::enter(const std::vector<std::string>& args)
{
	bool success = true;

	ResourceManager::LoadShader("res/shaders/post.vert", "res/shaders/post.frag", "post");
	ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");

	glm::mat4 proj = glm::ortho(0.0f, (float)(1920), (float)(1080), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("batch").Use().SetMatrix4("projection", proj);

	ResourceManager::LoadTexture("res/textures/logo_tm.png", true, GL_NEAREST, GL_LINEAR, "logo_tm");
	ResourceManager::LoadTexture("res/textures/logo_fb.png", true, GL_NEAREST, GL_LINEAR, "logo_fb");
	ResourceManager::LoadTexture("res/textures/logo_dw.png", true, GL_NEAREST, GL_LINEAR, "logo_dw");

	batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
	batchRenderer->initBatchRenderData();

	post = new PostProcessor(ResourceManager::GetShader("post"), 1920, 1080);

	return success;
}

void SplashState::network(int ms){}

void SplashState::update(float dt)
{
	if (tick <= keyframes.back().tick) {
	    post->fadeFactor = calculateFadeFactor(tick);
	}

	if(tick == 160)
		currentLogo = "logo_fb";
	if(tick == 320)
		currentLogo = "logo_dw";
	if(tick == 480 || isAnyKeyPressed())
		desiredState = 4;

	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

	if(1 == present)
	{
		int axesCount;
		const float *axes = glfwGetJoystickAxes( GLFW_JOYSTICK_1, &axesCount);

		int buttonCount;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		if(GLFW_PRESS == buttons[0])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[1])
		{
			desiredState = 1;
		}
		if(axes[5] == 1)
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[2])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[3])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[4])
		{
			desiredState = 1;
		}
		if(axes[4] == 1)
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[5])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[6])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[7])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[8])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[9])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[10])
		{
			desiredState = 1;
		}
		if(GLFW_PRESS == buttons[11])
		{
			desiredState = 1;
		}

		if(GLFW_PRESS == buttons[12])
		{
			desiredState = 1;
		}

		if(GLFW_PRESS == buttons[13])
		{
			desiredState = 1;
		}
	}
}

void SplashState::render()
{
	post->BeginRender();

	batchRenderer->ResetStats();
	batchRenderer->BeginBatch();

	batchRenderer->DrawQuad({0, 0}, {1920, 1080}, 0, ResourceManager::GetTexture(currentLogo).ID);

	batchRenderer->EndBatch();
	batchRenderer->Flush();

	post->EndRender();
    post->Render(glfwGetTime());
}


SplashState SplashState::sSplashState;

SplashState::SplashState()
{

}

SplashState* SplashState::get()
{
	return &sSplashState;
}
