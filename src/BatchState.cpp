#include "BatchState.h"

Renderer* batchRenderer;
Camera* camera;

float cX = 0;
float cY = 0;
float cR = 0.0f;
float cS = 1.0f;
float parallaxFactor = 0.0f;
	
bool BatchState::exit()
{
	batchRenderer->Delete();
	delete batchRenderer;

	return true;
}

bool BatchState::enter()
{
	bool success = true;

	camera = new Camera(0, 1920, 1080, 0);

	ResourceManager::LoadTexture("res/textures/obamium.jpg", false, "obama");
	ResourceManager::LoadTexture("res/textures/pop_cat.png", true, "popcat");


	ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");
	batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
	batchRenderer->initBatchRenderData();

	return success;
}

void BatchState::update(float dt)
{
	if (this->Keys[GLFW_KEY_W]){
		cY -= 70.0f;
	}
	if (this->Keys[GLFW_KEY_S]){
		cY += 70.0f;
	}
	if (this->Keys[GLFW_KEY_D]){
		cX += 70.0f;
	}
	if (this->Keys[GLFW_KEY_A]){
		cX -= 70.0f;
	}
	if (this->Keys[GLFW_KEY_Z]){
		cS -= .1f;
	}
	if (this->Keys[GLFW_KEY_X]){
		cS += .1f;
	}




}

void BatchState::render()
{
	camera->SetPosition(glm::vec3(cX, cY, 0));
	camera->SetRotation(cR);
	camera->SetScale(glm::vec3(cS));

	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", camera->GetViewProjectionMatrix());

	batchRenderer->ResetStats();
	batchRenderer->BeginBatch();

	for(float y = 0; y < 10000; y += 100)
	{
		for(float x = 0; x < 10000; x += 100)
		{
			float rot = (x + y) / 2000.0f;
			glm::vec4 color = { (x + 100) / 20000.0f, 0.2f, (y + 100) / 20000.0f, 1.0f};
			batchRenderer->DrawQuad({x, y}, {100.0f, 100.0f}, rot, color);
		}
	}

	for(int y = 0; y < 10000; y += 100)
	{
		for(int x = 10000; x < 50000; x += 100)
		{
			float rot = (x + y) / 200;
			GLuint tex = (x + y) % 200 == 0 ? ResourceManager::GetTexture("obama").ID : ResourceManager::GetTexture("popcat").ID;
			batchRenderer->DrawQuad({x, y}, {50.0f, 50.0f}, 0.0f, tex, camera->GetPosition(), parallaxFactor);
		}
	}


	batchRenderer->DrawQuad({0, 0}, {100.0f, 100.0f}, 45.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	batchRenderer->EndBatch();

	batchRenderer->Flush();

	const Stats& stats = batchRenderer->GetStats();
    ImGui::Text("Draw Calls: %d", stats.DrawCount);
    ImGui::Text("Quads Rendered: %d", stats.QuadCount);
    ImGui::SliderFloat("Parallax", &parallaxFactor, -1.0f, 1.0f);
}


BatchState BatchState::sBatchState;

BatchState::BatchState()
{

}

BatchState* BatchState::get()
{
	return &sBatchState;
}
