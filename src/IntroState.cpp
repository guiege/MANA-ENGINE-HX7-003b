#include "IntroState.h"

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

	shadowRenderer->Delete();
	delete shadowRenderer;

	// post->Delete();
	// delete post;

	return true;
}

bool IntroState::enter()
{
	//Loading success flag
	bool success = true;

	m_Camera = new Camera(0, 1920, 1080, 0);
	//0.3333(3x), 0.4(2.5x), 0.5(2x), 0.6667(1.5x), 1.0(1x)

    ResourceManager::LoadShader("res/shaders/post.vert", "res/shaders/post.frag", "post");
    ResourceManager::LoadShader("res/shaders/default.vert", "res/shaders/palette.frag", "palette");
    ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");
    ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/shadow.frag", "shadow");

    ResourceManager::LoadTexture("res/textures/hydesheetindexed.png", true, "hydesheet");
    ResourceManager::LoadTexture("res/textures/hydepal.png", true, "hydepal");
    ResourceManager::LoadTexture("res/textures/hydepalp2.png", true, "hydepalp2");
    ResourceManager::LoadTexture("res/textures/blackpal.png", true, "shadowpal");
    ResourceManager::LoadTexture("res/textures/ui/icons-capcom-32.png", true, GL_LINEAR, GL_NEAREST, "icons-capcom-32");
    ResourceManager::LoadTexture("res/textures/ui/gauge_00.png", GL_LINEAR, GL_LINEAR, true, "gauge");
    ResourceManager::LoadTexture("res/textures/pop_cat.png", true, "popcat");
    // ResourceManager::LoadTexture("res/textures/ryu.png", true, "ryusheet");
    

    proj = glm::ortho(0.0f, (float)(1920), (float)(1080), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("batch").Use().SetMatrix4("projection", proj);
    ResourceManager::GetShader("palette").Use().SetMatrix4("projection", proj);
    ResourceManager::GetShader("shadow").Use().SetMatrix4("projection", proj);

    characterRenderer = new Renderer(ResourceManager::GetShader("palette"));

    batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
    batchRenderer->initBatchRenderData();

    shadowRenderer = new Renderer(ResourceManager::GetShader("shadow"));

	// post = new PostProcessor(ResourceManager::GetShader("post"), 1920, 1080);
	inputHandler = new InputHandler();
	inputHandler2 = new InputHandler();

	icons = new Spritesheet(ResourceManager::GetTexture("icons-capcom-32"), "res/textures/ui/icons-capcom-32.json", 0, 0, 32, 32, 0);
	icons->SetFrame(0);

	gauge = new Spritesheet(ResourceManager::GetTexture("gauge"), "res/textures/ui/gauge_00.json", 0, 0, 1024, 1024, 0);
	gauge->SetFrame(0);
	gauge->SetScale({1.0f, 1.0f});

	testChar = new TestCharacter(inputHandler, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 1500, 0, 4296, 15673, 0, solids);
	testChar->init();
	// actors.push_back(testChar);

	testChar2 = new TestCharacter(inputHandler2, ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 2500, 0, 4296, 15673, 0, solids);
	testChar2->init();
	// actors.push_back(testChar2);

	Solid platform(0, 700, 800, 800, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	Solid stage(-1000, 980, 6000, 1000, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	Solid wallL(-40, 0, 40, 980, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	Solid wallR(4000, 0, 40, 980, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	// solids.push_back(platform);
	solids.push_back(stage);
	solids.push_back(wallL);
	solids.push_back(wallR);

	cameraXPos = 0;
	cameraYPos = 180;
	cameraRot = 0.0f;
	// cameraScale = 0.6f;

	// post->Chromatic = true;

	return success;
}

int IntroState::ReadInputs()
{
	int inputs = 0;

   	if(Keys[GLFW_KEY_U])
	{
		inputs |= ButtonIDs::LP;
	}
	if(Keys[GLFW_KEY_I])
	{
		inputs |= ButtonIDs::MP;
	}
	if(Keys[GLFW_KEY_O])
	{
		inputs |= ButtonIDs::HP;
	}

	if(Keys[GLFW_KEY_J])
	{
		inputs |= ButtonIDs::LK;
	}
	if(Keys[GLFW_KEY_K])
	{
		inputs |= ButtonIDs::MK;
	}
	if(Keys[GLFW_KEY_L])
	{
		inputs |= ButtonIDs::HK;
	}

	if (Keys[GLFW_KEY_D])
	{
		if(testChar2->GetFlipped())
			inputs |= ButtonIDs::BACK;
		else
			inputs |= ButtonIDs::FORWARD;
	}
	if (Keys[GLFW_KEY_A])
	{
		if(testChar2->GetFlipped())
			inputs |= ButtonIDs::FORWARD;
		else
			inputs |= ButtonIDs::BACK;
	}
	if (Keys[GLFW_KEY_S])
	{
		inputs |= ButtonIDs::DOWN;
	}
	if (Keys[GLFW_KEY_SPACE])
	{
		inputs |= ButtonIDs::UP;
	}

	return inputs;
}

int IntroState::ReadControllerInputs()
{
	int inputs = 0;
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);

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
			inputs |= ButtonIDs::LK;
		}

		if(GLFW_PRESS == buttons[1])
		{
			inputs |= ButtonIDs::MK;
		}
		if(axes[5] == 1)
		{
			inputs |= ButtonIDs::HK;
		}

		if(GLFW_PRESS == buttons[2])
		{
			inputs |= ButtonIDs::LP;
		}

		if(GLFW_PRESS == buttons[3])
		{
			inputs |= ButtonIDs::MP;
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
			inputs |= ButtonIDs::HP;
		}

		if(GLFW_PRESS == buttons[6])
		{
			std::cout << "select button pressed" << std::endl;
		}

		if(GLFW_PRESS == buttons[7])
		{
			inputs |= ButtonIDs::START;
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
			inputs |= ButtonIDs::UP;
		}

		if(GLFW_PRESS == buttons[11])
		{
			if(testChar->GetFlipped())
				inputs |= ButtonIDs::BACK;
			else
				inputs |= ButtonIDs::FORWARD;
		}

		if(GLFW_PRESS == buttons[12])
		{
			inputs |= ButtonIDs::DOWN;
		}

		if(GLFW_PRESS == buttons[13])
		{
			if(testChar->GetFlipped())
				inputs |= ButtonIDs::FORWARD;
			else
				inputs |= ButtonIDs::BACK;
		}
	}
	return inputs;
}

void IntroState::update(float dt)
{
	inputs[0] = ReadInputs();
	inputs[1] = ReadControllerInputs();
	std::cout << testChar->pos.y << std::endl;

	inputHandler2->registerInputs(inputs[0]);
	inputHandler->registerInputs(inputs[1]);

	if(Keys[GLFW_KEY_R])
	{
		testChar->pos.x = 1500;
		testChar2->pos.x = 2500;
		testChar->xCollision = false;
		testChar2->xCollision = false;
	}

	//FOR TESTING TRADES
	// if(Keys[GLFW_KEY_U])
	// 	{
	// 		inputHandler->registerInput(FK_Input_Buttons.LP);
	// 	}
	if(this->Keys[GLFW_KEY_LEFT])
	{
		cameraXPos -= 1;
		// solids[0].Move(-20, 0);
	}
	if(this->Keys[GLFW_KEY_RIGHT])
	{
		cameraXPos += 1;
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
		cameraScale -= .01;
	}
	if (this->Keys[GLFW_KEY_X]){
		cameraScale += .01;
	}

	if(testChar->GetRequestedShake() > 0.0f){
		trauma += testChar->GetRequestedShake();
		testChar->SetRequestedShake(0.0f);
	}

	if(testChar2->GetRequestedShake() > 0.0f){
		trauma += testChar2->GetRequestedShake();
		testChar2->SetRequestedShake(0.0f);
	}

	if(trauma > 1){
		trauma = 1;
	}

	if(trauma > 0)
		trauma -= 0.01;

	trauma = std::max(0.0f, std::min(trauma, 1.0f));

	float frequency = 0.40f;

	shake = (trauma * trauma);
	offsetX = maxOffset.x * shake * perlin.noise1D(tick * frequency);
	offsetY = maxOffset.y * shake * perlin.noise1D((tick + 1) * frequency);
	angle = maxAngle * shake * perlin.noise1D((tick + 2) * frequency);

	std::vector<int> inputNumbers;

	std::bitset<7> newButtonState(inputHandler->butPress.to_ullong());
	if((newButtonState & FK_Input_Buttons.LP) != 0) inputNumbers.push_back(8);
	if((newButtonState & FK_Input_Buttons.MP) != 0) inputNumbers.push_back(9);
	if((newButtonState & FK_Input_Buttons.HP) != 0) inputNumbers.push_back(10);
	if((newButtonState & FK_Input_Buttons.LK) != 0) inputNumbers.push_back(11);
	if((newButtonState & FK_Input_Buttons.MK) != 0) inputNumbers.push_back(12);
	if((newButtonState & FK_Input_Buttons.HK) != 0) inputNumbers.push_back(13);

	if(inputHandler->inputChanged){
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
	}

	if (!inputNumbers.empty()) {
	    currentIndex = (currentIndex + 1) % MAX_HISTORY_SIZE;
	    
	    inputHistory[currentIndex] = inputNumbers;
	    
	    if (inputHistory.size() > MAX_HISTORY_SIZE) {
	        int oldestIndex = (currentIndex + 1) % MAX_HISTORY_SIZE;
	        inputHistory.erase(oldestIndex);
	    }
	}

	if(!Keys[GLFW_KEY_LEFT_CONTROL] && (!Keys[GLFW_KEY_S] || !Keys[GLFW_KEY_L]))
	{
		savingLoading = false;
	}

	if(!savingLoading && Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_S]){
		save_char();
		savingLoading = true;
	}

	if(!savingLoading && Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_L]){
		load_char();
		savingLoading = true;
	}

	testChar->updateScript(tick, testChar2);
	testChar2->updateScript(tick, testChar);
	std::string char1state = testChar->GetCurrentState();
	std::string char2state = testChar2->GetCurrentState();

	// std::cout << plusframestimer <<std::endl;
	// if(char2state == "CmnActStand"){
	// 	plusframestimer=0;
	// } else{
	// 	if(char1state == "CmnActStand")
	// 		plusframestimer++;
	// }

	if(testChar->checkCollision(testChar2))
	{
		testChar->hitOpponent(testChar2, char1state.c_str());
	}
	if(testChar2->checkCollision(testChar))
	{
		testChar2->hitOpponent(testChar, char2state.c_str());
	}

	// Solid wallL(-40, 0, 40, 980, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	// Solid wallR(4000, 0, 40, 980, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	//Solid floor(0, 980, 4000, 1000, 0, actors, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
	// if(testChar->pos.x >= 3400)
	// 	testChar->pos.x = 3400;
	// if(testChar->pos.x <= -400)
	// 	testChar->pos.x = -400;
	// if(testChar2->pos.x >= 3400)
	// 	testChar2->pos.x = 3400;
	// if(testChar2->pos.x <= -400)
	// 	testChar2->pos.x = -400;
	// std::cout << testChar->pos.x << std::endl;

	if(testChar->GetHitstop() == 0){
		if(!testChar->isActionable())
			p1UnactionableTimer++;
		else{
			if(p1UnactionableTimer != 0)
				p1Unactionable = p1UnactionableTimer;
			p1UnactionableTimer = 0;
		}

		if(testChar->isHitboxActive())
			p1AtkActiveTimer++;
		else{
			if(p1AtkActiveTimer != 0)
				p1AtkActive = p1AtkActiveTimer;
			p1AtkActiveTimer = 0;
		}
	}

	if(testChar2->GetHitstop() == 0){ //horrible fucking code fix it
		if(!testChar2->isActionable())
			p2UnactionableTimer++;
		else{
			if(p2UnactionableTimer != 0)
				p2Unactionable = p2UnactionableTimer;
			p2UnactionableTimer = 0;
		}

		if(testChar2->isHitboxActive())
			p2AtkActiveTimer++;
		else{
			if(p2AtkActiveTimer != 0)
				p2AtkActive = p2AtkActiveTimer;
			p2AtkActiveTimer = 0;
		}
	}

	// std::cout << "Active: " << p2AtkActive << "/ " << "Advantage: " << (p1Unactionable - p2Unactionable) << std::endl;

	inputHandler->update(tick);
	inputHandler2->update(tick);

	float distance = testChar2->GetCenterPos().x - testChar->GetCenterPos().x;
	float distanceY = testChar2->GetCenterPos().y - testChar->GetCenterPos().y;

	cameraXPos += (abs(testChar2->GetCenterPos().x - (distance)/2) - cameraXPos) * cameraSnap;
	cameraYPos += (abs(testChar2->GetCenterPos().y - (distanceY)/2) - cameraYPos) * cameraSnap;

	if(cameraXPos < cameraMinPos)
		cameraXPos = cameraMinPos;

	if(cameraXPos > cameraMaxPos)
		cameraXPos = cameraMaxPos;

    float baseScale = 0.6f;
    float maxScale = 2.0f;
    float scale = 0.8f;
    if(abs(distance) > 960)
    	scale = baseScale + (abs(distance) / 5000.0f);
    
    cameraScale += (glm::clamp(scale, baseScale, maxScale) - cameraScale) * .1f;



}

void IntroState::render()
{
	//Camera Params for Scene Rendering
	m_Camera->SetCenter(glm::vec3(960, 540, 0));
	m_Camera->SetCenterPosition(glm::vec3(cameraXPos + offsetX, cameraYPos + offsetY - (380 * cameraScale), 0));
	m_Camera->SetRotation(angle);
	m_Camera->SetScale(cameraScale);
	worldProj = m_Camera->GetViewProjectionMatrix();
	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", worldProj);//Set Projection Matrix to World
	ResourceManager::GetShader("palette").Use().SetMatrix4("projection", worldProj);

	//Camera Params for UI
	m_Camera->SetPosition(glm::vec3(0, 0, 0));
	m_Camera->SetRotation(0);
	m_Camera->SetCenter(glm::vec3(0, 0, 0));
	m_Camera->SetScale(0.6667); //1.5x scale because mbtl assets are for 720p
	uiProj = m_Camera->GetViewProjectionMatrix();

	// post->BeginRender();

	batchRenderer->ResetStats();

	batchRenderer->BeginBatch(); //BG pass

	for (auto& solid : solids){
		solid.draw(batchRenderer);
	}

	// for(float y = 0; y < 10000; y += 100)
	// {
	// 	for(float x = 0; x < 10000; x += 100)
	// 	{
	// 		float rot = (x + y) / 2000.0f;
	// 		glm::vec4 color = { (x + 100) / 20000.0f, 0.2f, (y + 100) / 20000.0f, 1.0f};
	// 		batchRenderer->DrawQuad({x, y}, {100.0f, 100.0f}, 0.0f, color);
	// 	}
	// }

    batchRenderer->EndBatch();
	batchRenderer->Flush(); //End BG pass

	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", uiProj); //Set Projection Matrix to UI

	batchRenderer->BeginBatch(); //UI Pass

	float percentage = 1 - (testChar2->GetHealth() / 420);
	int value = (420 - testChar2->GetHealth());

	gauge->SetFrame(0);
	gauge->pos.x = 0;
	gauge->draw(batchRenderer);
	gauge->SetFrame(1);
	gauge->draw(batchRenderer);
	gauge->SetFrame(2);
	gauge->draw(batchRenderer);
	gauge->SetFrame(3);
	gauge->pos = {150, 58};
	gauge->drawclip(batchRenderer, 0, 322, testChar2->GetHealth(), 20, false);
	gauge->pos = {0, 0};
	gauge->SetFrame(4);
	gauge->draw(batchRenderer);

	// batchRenderer->DrawQuad({100, 50}, {(testChar2->GetHealth() / 420.0f) * 100.0f, 50.0f}, 0, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	for (int i = 0; i < MAX_HISTORY_SIZE; ++i) {
	    int index = (currentIndex - i + MAX_HISTORY_SIZE) % MAX_HISTORY_SIZE;

	    if (inputHistory.find(index) != inputHistory.end()) {
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
	batchRenderer->Flush(); //End UI Pass

	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", worldProj); //Set Projection Matrix to World
	ResourceManager::GetShader("palette").Use().SetMatrix4("projection", worldProj);
	ResourceManager::GetShader("shadow").Use().SetMatrix4("projection", worldProj);

	// float skew = 0.0f;
	// glm::vec2 char1centerpos = testChar
	// int bottompos = (ypos +(ypos /2));
	// shadowRenderer->DrawTexture(ResourceManager::GetTexture("popcat"), testChar->pos.x, testChar->pos.y, 960, testChar->pos.y, 960 + (100 * skew), bottompos, (100 * skew), bottompos, 0.12, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));

	batchRenderer->BeginBatch(); //Character Pass

	testChar->draw(batchRenderer, characterRenderer, ResourceManager::GetTexture("hydepal"), ResourceManager::GetTexture("shadowpal"));

    testChar2->draw(batchRenderer, characterRenderer, ResourceManager::GetTexture("hydepalp2"),ResourceManager::GetTexture("shadowpal"));

    batchRenderer->EndBatch();
	batchRenderer->Flush(); //End Character Pass

	// const Stats& stats = batchRenderer->GetStats();
    // ImGui::Text("Draw Calls: %d", stats.DrawCount);
    // ImGui::Text("Quads Rendered: %d", stats.QuadCount);


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