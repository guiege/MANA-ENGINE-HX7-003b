#include "IntroState.h"

std::vector<Solid> solids;
std::vector<Actor*> actors;

int testCharFrame = 0;

// PostProcessor *post;

int crouchingjabtimer = 0;


std::vector<int> spdH{-1, 1, 1, 1};
std::vector<std::bitset<4>> spdC{FK_Input_Buttons.FORWARD, FK_Input_Buttons.DOWN, FK_Input_Buttons.BACK, FK_Input_Buttons.UP};

bool IntroState::exit()
{
	delete testChar;
	delete testChar2;
	delete icons;
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
	cameraScale = 0.75f;

    ResourceManager::LoadShader("res/shaders/post.vert", "res/shaders/post.frag", "post");
    ResourceManager::LoadShader("res/shaders/default.vert", "res/shaders/palette.frag", "palette");
    ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");

    ResourceManager::LoadTexture("res/textures/hydesheetindexed.png", true, "hydesheet");
    ResourceManager::LoadTexture("res/textures/hydepal.png", true, "hydepal");
    ResourceManager::LoadTexture("res/textures/hydepalp2.png", true, "hydepalp2");
    ResourceManager::LoadTexture("res/textures/glyphs/icons-capcom-32.png", true, "icons-capcom-32");
    // ResourceManager::LoadTexture("res/textures/ryu.png", true, "ryusheet");
    // ResourceManager::LoadTexture("res/textures/ffviir-zoom-midgar-city.jpg", false, "midgar");
    

    proj = glm::ortho(0.0f, (float)(1920), (float)(1080), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("batch").Use().SetMatrix4("projection", proj);
    ResourceManager::GetShader("palette").Use().SetMatrix4("projection", proj);

    characterRenderer = new Renderer(ResourceManager::GetShader("palette"));

    batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
    batchRenderer->initBatchRenderData();

	// post = new PostProcessor(ResourceManager::GetShader("post"), 1920, 1080);
	inputHandler = new InputHandler();
	inputHandler2 = new InputHandler();

	icons = new Spritesheet(ResourceManager::GetTexture("icons-capcom-32"), "res/textures/glyphs/icons-capcom-32.json", 0, 0, 32, 32, 0);
	icons->SetFrame(0);

	testChar = new TestCharacter(inputHandler, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", -950, 0, 4296, 15673, 0, solids);
	testChar->init();
	actors.push_back(testChar);

	testChar2 = new TestCharacter(inputHandler2, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 50, 0, 4296, 15673, 0, solids);
	testChar2->init();
	testChar2->SetFlipped(true);
	actors.push_back(testChar2);

	Solid platform(1000, 700, 800, 800, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	Solid floor(-2000, 980, 4000, 1000, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	// solids.push_back(platform);
	solids.push_back(floor);

	// post->Chromatic = true;

	return success;
}

void IntroState::update(float dt)
{
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	// std::cout << "Stick 1 is here: " << present << std::endl;

	if(1 == present)
	{
		int axesCount;
		const float *axes = glfwGetJoystickAxes( GLFW_JOYSTICK_1, &axesCount);

		// for (int i = 0; i < axesCount; i++) {
		//     std::cout << "Axis " << i << ": " << std::fixed << std::setprecision(6) << axes[i] << std::endl;
		// }
		// std::cout << "\033[2J\033[1;1H";
		// std::cout << "Number of axes available" << axesCount << std::endl;

		// std::cout << "Left Stick X Axis: " << axes[0] << std::endl;
		// std::cout << "Left Stick Y Axis: " << axes[1] << std::endl;
		// std::cout << "Right Stick X Axis: " << axes[2] << std::endl;
		// std::cout << "Right Stick Y Axis: " << axes[3] << std::endl;
		// std::cout << "Left Trigger: " << axes[4] << std::endl;
		// std::cout << "Right Trigger: " << axes[5] << std::endl;

		int buttonCount;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

		// for (int i = 0; i < buttonCount; i++) {
		//     std::cout << "Button " << i << ": " << (buttons[i] == GLFW_PRESS ? "Pressed" : "Released") << std::endl;
		// }

		if(GLFW_PRESS == buttons[0])
		{
			inputHandler->registerInput(FK_Input_Buttons.LK);
		}

		if(GLFW_PRESS == buttons[1])
		{
			inputHandler->registerInput(FK_Input_Buttons.MK);
		}
		if(axes[5] == 1)
		{
			inputHandler->registerInput(FK_Input_Buttons.HK);
		}

		if(GLFW_PRESS == buttons[2])
		{
			inputHandler->registerInput(FK_Input_Buttons.LP);
		}

		if(GLFW_PRESS == buttons[3])
		{
			inputHandler->registerInput(FK_Input_Buttons.MP);
		}

		if(GLFW_PRESS == buttons[4])
		{
			std::cout << "far right punch button pressed" << std::endl;
		}
		if(axes[4] == 1)
		{
			std::cout << "far right kick button pressed" << std::endl;
		}

		if(GLFW_PRESS == buttons[5])
		{
			inputHandler->registerInput(FK_Input_Buttons.HP);
		}

		if(GLFW_PRESS == buttons[6])
		{
			std::cout << "select button pressed" << std::endl;
		}

		if(GLFW_PRESS == buttons[7])
		{
			inputHandler->registerInput(FK_Input_Buttons.START);
		}

		if(GLFW_PRESS == buttons[8])
		{
			std::cout << "far left nub pressed" << std::endl;
		}

		if(GLFW_PRESS == buttons[9])
		{
			std::cout << "9 pressed" << std::endl;
		}

		if(GLFW_PRESS == buttons[10])
		{
			inputHandler->registerInput(FK_Input_Buttons.UP);
		}

		if(GLFW_PRESS == buttons[11])
		{
			if(testChar->GetFlipped())
				inputHandler->registerInput(FK_Input_Buttons.BACK);
			else
				inputHandler->registerInput(FK_Input_Buttons.FORWARD);
		}

		if(GLFW_PRESS == buttons[12])
		{
			inputHandler->registerInput(FK_Input_Buttons.DOWN);
		}

		if(GLFW_PRESS == buttons[13])
		{
			if(testChar->GetFlipped())
				inputHandler->registerInput(FK_Input_Buttons.FORWARD);
			else
				inputHandler->registerInput(FK_Input_Buttons.BACK);
		}
	}

	if(this->Keys[GLFW_KEY_F])
		testChar->SetFlipped(true);
	if(this->Keys[GLFW_KEY_G])
		testChar->SetFlipped(false);

	if(Keys[GLFW_KEY_U])
	{
		inputHandler2->registerInput(FK_Input_Buttons.LP);
	}
	if(Keys[GLFW_KEY_I])
	{
		inputHandler2->registerInput(FK_Input_Buttons.MP);
	}
	if(Keys[GLFW_KEY_O])
	{
		inputHandler2->registerInput(FK_Input_Buttons.HP);
	}

	if(Keys[GLFW_KEY_J])
	{
		inputHandler2->registerInput(FK_Input_Buttons.LK);
	}
	if(Keys[GLFW_KEY_K])
	{
		inputHandler2->registerInput(FK_Input_Buttons.MK);
	}
	if(Keys[GLFW_KEY_L])
	{
		inputHandler2->registerInput(FK_Input_Buttons.HK);
	}

	if (this->Keys[GLFW_KEY_D])
	{
		if(testChar2->GetFlipped())
			inputHandler2->registerInput(FK_Input_Buttons.BACK);
		else
			inputHandler2->registerInput(FK_Input_Buttons.FORWARD);
	}
	if (this->Keys[GLFW_KEY_A])
	{
		if(testChar2->GetFlipped())
			inputHandler2->registerInput(FK_Input_Buttons.FORWARD);
		else
			inputHandler2->registerInput(FK_Input_Buttons.BACK);
	}
	if (this->Keys[GLFW_KEY_S])
	{
		inputHandler2->registerInput(FK_Input_Buttons.DOWN);
	}
	if (this->Keys[GLFW_KEY_SPACE])
	{
		inputHandler2->registerInput(FK_Input_Buttons.UP);
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
	// //check shoryu first then command normals and then hadou
	// if(inputHandler->checkCommand(FK_Input_Buttons.DOWN_FORWARD, true) || inputHandler->checkCommand(FK_Input_Buttons.DOWN, true))
	// {
	// 	if(inputHandler->checkCommand(FK_Input_Buttons.LP, false)){
	// 		crouchingjabtimer = 10;
	// 		std::cout << "LOW" << std::endl;
	// 	}
	// }

	// if(inputHandler->checkCommand(spd))
	// {
	// 	std::cout << "SPD" << std::endl;
	// 	if(inputHandler->checkCommand(FK_Input_Buttons.LP, false))
	// 		testChar->MoveY(250);
	// } 
	// else if(inputHandler->checkCommand(hadoken))
	// {
	// 	std::cout << "HADOKEN" << std::endl;
	// 	if(inputHandler->checkCommand(FK_Input_Buttons.LP, false) && crouchingjabtimer == 0)
	// 		testChar->MoveX(250);
	// }

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

	std::vector<int> inputNumbers;

	std::bitset<7> newButtonState(inputHandler->butPress.to_ullong());
	if((newButtonState & FK_Input_Buttons.LP) != 0) inputNumbers.push_back(8);
	if((newButtonState & FK_Input_Buttons.MP) != 0) inputNumbers.push_back(9);
	if((newButtonState & FK_Input_Buttons.HP) != 0) inputNumbers.push_back(10);
	if((newButtonState & FK_Input_Buttons.LK) != 0) inputNumbers.push_back(11);
	if((newButtonState & FK_Input_Buttons.MK) != 0) inputNumbers.push_back(12);
	if((newButtonState & FK_Input_Buttons.HK) != 0) inputNumbers.push_back(13);

	if (inputHandler->currentDirection == "UP") {
	    inputNumbers.push_back(2);
	} if (inputHandler->currentDirection == "BACK") {
	    inputNumbers.push_back(0);
	} if (inputHandler->currentDirection == "FORWARD") {
		inputNumbers.push_back(1);
	} if (inputHandler->currentDirection == "DOWN") {
	    inputNumbers.push_back(3);
	} if (inputHandler->currentDirection == "UP_FORWARD") {
	    inputNumbers.push_back(5);
	} if (inputHandler->currentDirection == "UP_BACK") {
	    inputNumbers.push_back(4);
	} if (inputHandler->currentDirection == "DOWN_BACK") {
	    inputNumbers.push_back(6);
	} if (inputHandler->currentDirection == "DOWN_FORWARD") {
	    inputNumbers.push_back(7);
	} 

	if (inputHistory.empty() || inputHistory[currentIndex] != inputNumbers || inputHandler->inputChanged) {
	    currentIndex = (currentIndex + 1) % MAX_HISTORY_SIZE;
	    
	    inputHistory[currentIndex] = inputNumbers;
	    
	    if (inputHistory.size() > MAX_HISTORY_SIZE) {
	        int oldestIndex = (currentIndex + 1) % MAX_HISTORY_SIZE;
	        inputHistory.erase(oldestIndex);
	    }
	}

	testChar->update(tick);
	testChar2->update(tick);
	testChar->scriptSubroutine(tick, testChar2);
	testChar2->scriptSubroutine(tick, testChar);
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

	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", proj);

	batchRenderer->BeginBatch();

	for (int i = 0; i < MAX_HISTORY_SIZE; ++i) {
	    int index = (currentIndex - i + MAX_HISTORY_SIZE) % MAX_HISTORY_SIZE;
	    
	    if (inputHistory.find(index) != inputHistory.end() && !inputHistory[index].empty()) {
	        icons->pos.y = i * 32 + dirHistoryPos.y;
	        
	        // Sort the input numbers from least to greatest
	        std::vector<int> sortedInputs = inputHistory[index];
	        // std::sort(sortedInputs.begin(), sortedInputs.end(), std::greater<>());
	        
	        int drawnCount = 0;
	        for (auto it = sortedInputs.rbegin(); it != sortedInputs.rend() && drawnCount < 4; ++it) {
	            int frameNumber = *it;
	            
				icons->pos.x = dirHistoryPos.x + (drawnCount * 32);

	            icons->SetFrame(frameNumber);
	            icons->draw(batchRenderer);
	            
	            drawnCount++;
	        }
	    }
	}

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
