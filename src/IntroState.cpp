#include "IntroState.h"

Character* testChar;
Character* testChar2;
InputHandler* inputHandler;

std::vector<Solid> solids;
std::vector<Actor*> actors;

int testCharFrame = 0;

// PostProcessor *post;

std::vector<int> hdknH{-1, 1};
std::vector<std::bitset<4>> hdknC{FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD};

std::vector<int> sykH{-1, 1, 1};
std::vector<std::bitset<4>> sykC{FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.FORWARD};

std::vector<int> spdH{-1, 1, 1, 1};
std::vector<std::bitset<4>> spdC{FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP};

CommandSequence hadoken(hdknC, hdknH, 8);
CommandSequence shoryuken(sykC, sykH, 8);
CommandSequence spd(spdC, spdH, 8);

std::ifstream file("res/hitboxes/testAnims.json");

json testAnims = json::parse(file);

const auto& anim = testAnims["animationList"][0];
const auto& jab = testAnims["animationList"][1];

std::string animName = anim["animationName"];
std::vector<int> frames = anim["frames"].get<std::vector<int>>();
std::vector<int> keyframes = anim["keyframes"].get<std::vector<int>>();
bool repeat = anim["repeat"];

Animation testAnim(keyframes, frames, repeat);
Animation jabAnim(jab["keyframes"], jab["frames"], jab["repeat"]);

int crouchingjabtimer = 0;

bool IntroState::exit()
{
    for (int i = actors.size() - 1; i >= 0; --i)
    {
        if (actors[i] != nullptr)
        {
            delete actors[i];
            actors[i] = nullptr;
        }
    }
	actors.clear();
	solids.clear();

	delete inputHandler;

	batchRenderer->Delete();	
	delete batchRenderer;

	// post->Delete();
	// delete post;

	return true;
}

bool IntroState::enter()
{
	//Loading success flag
	bool success = true;

	testCharFrame = 0;

	m_Camera = new Camera(0, 1920, 1080, 0);
	cameraXPos = 0;
	cameraYPos = 0;
	cameraRot = 0.0f;
	cameraScale = 1.0f;

    ResourceManager::LoadShader("res/shaders/post.vert", "res/shaders/post.frag", "post");
    ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");

    ResourceManager::LoadTexture("res/textures/hyde.png", true, "hyde");
    ResourceManager::LoadTexture("res/textures/hydesheet.png", true, "hydesheet");
    ResourceManager::LoadTexture("res/textures/ryu.png", true, "ryusheet");
    ResourceManager::LoadTexture("res/textures/ffviir-zoom-midgar-city.jpg", false, "midgar");
    

    glm::mat4 proj = glm::ortho(0.0f, (float)(1920), (float)(1080), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("testShader").Use().SetMatrix4("projection", proj);
    ResourceManager::GetShader("batch").Use().SetMatrix4("projection", proj);

    batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
    batchRenderer->initBatchRenderData();

	// post = new PostProcessor(ResourceManager::GetShader("post"), 1920, 1080);
	inputHandler = new InputHandler();

	testChar = new Character(ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 0, 0, 4296, 15673, 0, solids);
	testChar->init();
	testChar->PlayAnimation(testAnim);
	actors.push_back(testChar);

	testChar2 = new Character(ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 500, 0, 4296, 15673, 0, solids);
	testChar2->init();
	testChar2->SetFlipped(true);
	actors.push_back(testChar2);

	Solid platform(1000, 700, 800, 800, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	Solid floor(0, 980, 1920, 100, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	solids.push_back(platform);
	solids.push_back(floor);

	// post->Chromatic = true;

	return success;
}

void IntroState::update(float dt)
{
	// testChar->update(tick);
	testChar->animate(tick);
	// testChar2->update(tick);
	// testChar->SetFrame(testCharFrame);
	testChar->MoveY(10);

	if(this->Keys[GLFW_KEY_F])
		testChar->SetFlipped(true);
	if(this->Keys[GLFW_KEY_G])
		testChar->SetFlipped(false);

	if (this->Keys[GLFW_KEY_D])
	{
		inputHandler->registerInput(FK_Input_Buttons.FORWARD);
		testChar->MoveX(20);
	}
	if (this->Keys[GLFW_KEY_A])
	{
		inputHandler->registerInput(FK_Input_Buttons.BACK);
		testChar->MoveX(-20);
	}
	if (this->Keys[GLFW_KEY_S])
	{
		inputHandler->registerInput(FK_Input_Buttons.DOWN);
		testChar->MoveY(20);
	}
	if (this->Keys[GLFW_KEY_W])
	{
		inputHandler->registerInput(FK_Input_Buttons.UP);
		testChar->MoveY(-100);
	}
	if(this->Keys[GLFW_KEY_LEFT])
	{
		cameraXPos -= 5;
		// solids[0].Move(-20, 0);
	}
	if(this->Keys[GLFW_KEY_RIGHT])
	{
		cameraXPos += 5;
		// solids[0].Move(20, 0);
	}
	if(this->Keys[GLFW_KEY_UP])
	{
		cameraYPos -= 5;
		// solids[0].Move(0, -20);
	}
	if(this->Keys[GLFW_KEY_DOWN])
	{
		cameraYPos += 5;
		// solids[0].Move(0, 20);
	}
	if (this->Keys[GLFW_KEY_Z]){
		cameraScale -= .1;
	}
	if (this->Keys[GLFW_KEY_X]){
		cameraScale += .1;
	}

	if(Keys[GLFW_KEY_U])
	{
		inputHandler->registerInput(FK_Input_Buttons.LP);
		testChar->PlayAnimation(Animation(jab["keyframes"], jab["frames"], jab["repeat"]));
	}
	
	//check shoryu first then command normals and then hadou
	if(inputHandler->checkCommand(FK_Input_Buttons.DOWN_FORWARD, true) || inputHandler->checkCommand(FK_Input_Buttons.DOWN, true))
	{
		if(inputHandler->checkCommand(FK_Input_Buttons.LP, false)){
			crouchingjabtimer = 10;
			std::cout << "LOW" << std::endl;
		}
	}

	if(inputHandler->checkCommand(spd))
	{
		std::cout << "SPD" << std::endl;
		if(inputHandler->checkCommand(FK_Input_Buttons.LP, false))
			testChar->MoveY(250);
	} 
	else if(inputHandler->checkCommand(hadoken))
	{
		std::cout << "HADOKEN" << std::endl;
		if(inputHandler->checkCommand(FK_Input_Buttons.LP, false) && crouchingjabtimer == 0)
			testChar->MoveX(250);
	}

	if(this->Keys[GLFW_KEY_N]){
		testCharFrame--;
	}
	if(this->Keys[GLFW_KEY_M]){
		testCharFrame++;
	}

	inputHandler->update(tick);
	// std::cout << inputHandler->currentInputTimer  << " " << inputHandler->currentDirection << std::endl;

	if(crouchingjabtimer > 0)
		crouchingjabtimer--;

}

void IntroState::render()
{
	m_Camera->SetPosition(glm::vec3(cameraXPos, cameraYPos, 0));
	m_Camera->SetRotation(cameraRot);
	m_Camera->SetScale(glm::vec3(cameraScale, cameraScale, cameraScale));

	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());
	// post->BeginRender();

	batchRenderer->ResetStats();
	batchRenderer->BeginBatch();



	for (auto& solid : solids){
		solid.draw(batchRenderer);
	}
	// for (int i = 0; i < actors.size(); i++){
    // 	actors[i]->draw(renderer);
    // }

    testChar->draw(batchRenderer);
    // testChar2->draw(batchRenderer);

    batchRenderer->EndBatch();

	batchRenderer->Flush();

	const Stats& stats = batchRenderer->GetStats();
    ImGui::Text("Draw Calls: %d", stats.DrawCount);
    ImGui::Text("Quads Rendered: %d", stats.QuadCount);


	// post->EndRender();
    // post->Render(glfwGetTime());
}

//Declare static instance
IntroState IntroState::sIntroState;

IntroState::IntroState()
{
	//No public instantiation
}

IntroState* IntroState::get()
{
	//Get static instance
	return &sIntroState;
}
