#include "IntroState.h"

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

int crouchingjabtimer = 0;

bool IntroState::exit()
{
	delete testChar;
	delete testChar2;
    // for (int i = actors.size() - 1; i >= 0; --i)
    // {
    //     if (actors[i] != nullptr)
    //     {
    //         delete actors[i];
    //         actors[i] = nullptr;
    //     }
    // }
	actors.clear();
	solids.clear();

	delete inputHandler;
	delete inputHandler2;

	batchRenderer->Delete();	
	delete batchRenderer;

	characterRenderer->Delete();
	delete characterRenderer;

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
	cameraXPos = -960;
	cameraYPos = 0;
	cameraRot = 0.0f;
	cameraScale = 1.0f;

    ResourceManager::LoadShader("res/shaders/post.vert", "res/shaders/post.frag", "post");
    ResourceManager::LoadShader("res/shaders/default.vert", "res/shaders/palette.frag", "palette");
    ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");

    ResourceManager::LoadTexture("res/textures/hydesheetindexed.png", true, "hydesheet");
    ResourceManager::LoadTexture("res/textures/hydepal.png", true, "hydepal");
    ResourceManager::LoadTexture("res/textures/hydepalp2.png", true, "hydepalp2");
    ResourceManager::LoadTexture("res/textures/ryu.png", true, "ryusheet");
    ResourceManager::LoadTexture("res/textures/ffviir-zoom-midgar-city.jpg", false, "midgar");
    

    glm::mat4 proj = glm::ortho(0.0f, (float)(1920), (float)(1080), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("batch").Use().SetMatrix4("projection", proj);
    ResourceManager::GetShader("palette").Use().SetMatrix4("projection", proj);

    characterRenderer = new Renderer(ResourceManager::GetShader("palette"));

    batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
    batchRenderer->initBatchRenderData();

	// post = new PostProcessor(ResourceManager::GetShader("post"), 1920, 1080);
	inputHandler = new InputHandler();
	inputHandler2 = new InputHandler();

	testChar = new TestCharacter(inputHandler, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", -950, 0, 4296, 15673, 0, solids);
	testChar->init();
	// actors.push_back(testChar);

	testChar2 = new TestCharacter(inputHandler2, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 50, 0, 4296, 15673, 0, solids);
	testChar2->init();
	testChar2->SetFlipped(true);
	// actors.push_back(testChar2);

	Solid platform(1000, 700, 800, 800, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	Solid floor(-2000, 980, 4000, 100, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	solids.push_back(platform);
	solids.push_back(floor);

	// post->Chromatic = true;

	return success;
}

void IntroState::update(float dt)
{
	if(this->Keys[GLFW_KEY_F])
		testChar->SetFlipped(true);
	if(this->Keys[GLFW_KEY_G])
		testChar->SetFlipped(false);

	if(Keys[GLFW_KEY_J])
	{
		inputHandler2->registerInput(FK_Input_Buttons.BACK);
	}
	if(Keys[GLFW_KEY_L])
	{
		inputHandler2->registerInput(FK_Input_Buttons.FORWARD);
	}
	if(Keys[GLFW_KEY_I])
	{
		inputHandler2->registerInput(FK_Input_Buttons.UP);
	}
	if(Keys[GLFW_KEY_K])
	{
		inputHandler2->registerInput(FK_Input_Buttons.DOWN);
	}

	if(Keys[GLFW_KEY_LEFT_BRACKET])
	{
		inputHandler2->registerInput(FK_Input_Buttons.LP);
	}

	if (this->Keys[GLFW_KEY_D])
	{
		inputHandler->registerInput(FK_Input_Buttons.FORWARD);
	}
	if (this->Keys[GLFW_KEY_A])
	{
		inputHandler->registerInput(FK_Input_Buttons.BACK);
	}
	if (this->Keys[GLFW_KEY_S])
	{
		inputHandler->registerInput(FK_Input_Buttons.DOWN);
	}
	if (this->Keys[GLFW_KEY_W])
	{
		inputHandler->registerInput(FK_Input_Buttons.UP);
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

	if(Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_S]){
		save_char();
	}

	if(Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_L]){
		load_char();
	}

	if(Keys[GLFW_KEY_U])
	{
		inputHandler->registerInput(FK_Input_Buttons.LP);
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
	// std::cout << inputHandler->currentInputTimer  << " " << inputHandler->currentDirection << std::endl;

	if(crouchingjabtimer > 0)
		crouchingjabtimer--;

	testChar->MoveY(10);
	testChar2->MoveY(10);

	testChar->update(tick);
	testChar2->update(tick);
	testChar->animate(tick);
	// testChar2->animate(tick);
	inputHandler->update(tick);
	inputHandler2->update(tick);
}

void IntroState::render()
{
	m_Camera->SetPosition(glm::vec3(cameraXPos, cameraYPos, 0));
	m_Camera->SetRotation(cameraRot);
	m_Camera->SetScale(glm::vec3(cameraScale, cameraScale, cameraScale));

	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());
	ResourceManager::GetShader("palette").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());
	// post->BeginRender();

	batchRenderer->ResetStats();
	batchRenderer->BeginBatch();



	for (auto& solid : solids){
		solid.draw(batchRenderer);
	}
	// for (int i = 0; i < actors.size(); i++){
    // 	actors[i]->draw(renderer);
    // }

    testChar->draw(batchRenderer, characterRenderer, ResourceManager::GetTexture("hydepal"));
    testChar2->draw(batchRenderer, characterRenderer, ResourceManager::GetTexture("hydepalp2"));

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
