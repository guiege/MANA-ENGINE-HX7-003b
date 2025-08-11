#include "IntroState.h"

// #define SYNC_TEST

GameState gs = {  };
NonGameState ngs = { 0 };
GGPOSession *ggpo = NULL;

int
fletcher32_checksum(short *data, size_t len)
{
   int sum1 = 0xffff, sum2 = 0xffff;

   while (len) {
      size_t tlen = len > 360 ? 360 : len;
      len -= tlen;
      do {
         sum1 += *data++;
         sum2 += sum1;
      } while (--tlen);
      sum1 = (sum1 & 0xffff) + (sum1 >> 16);
      sum2 = (sum2 & 0xffff) + (sum2 >> 16);
   }

   /* Second reduction step to reduce sums to 16 bits */
   sum1 = (sum1 & 0xffff) + (sum1 >> 16);
   sum2 = (sum2 & 0xffff) + (sum2 >> 16);
   return sum2 << 16 | sum1;
}

bool __cdecl
vw_begin_game_callback(const char *)
{
   return true;
}

bool __cdecl
vw_on_event_callback(GGPOEvent *info)
{
   int progress;
   switch (info->code) {
   case GGPO_EVENTCODE_CONNECTED_TO_PEER:
      ngs.SetConnectState(info->u.connected.player, Synchronizing);
      break;
   case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
      progress = 100 * info->u.synchronizing.count / info->u.synchronizing.total;
      ngs.UpdateConnectProgress(info->u.synchronizing.player, progress);
      break;
   case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
      ngs.UpdateConnectProgress(info->u.synchronized.player, 100);
      break;
   case GGPO_EVENTCODE_RUNNING:
      ngs.SetConnectState(Running);
      // renderer->SetStatusText("");
      break;
   case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
      ngs.SetDisconnectTimeout(info->u.connection_interrupted.player,
                               timeGetTime(),
                               info->u.connection_interrupted.disconnect_timeout);
      break;
   case GGPO_EVENTCODE_CONNECTION_RESUMED:
      ngs.SetConnectState(info->u.connection_resumed.player, Running);
      break;
   case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
      ngs.SetConnectState(info->u.disconnected.player, Disconnected);
      break;
   case GGPO_EVENTCODE_TIMESYNC:
      Sleep(1000 * info->u.timesync.frames_ahead / 60);
      break;
   }
   return true;
}

bool __cdecl
vw_advance_frame_callback(int)
{
   int inputs[MAX_SHIPS] = { 0 };
   int disconnect_flags;

   ggpo_synchronize_input(ggpo, (void *)inputs, sizeof(int) * MAX_SHIPS, &disconnect_flags);
   IntroState::get()->VectorWar_AdvanceFrame(inputs, disconnect_flags);
   return true;
}

bool __cdecl
vw_load_game_state_callback(unsigned char *buffer, int len)
{
   memcpy(&gs, buffer, len);
   return true;
}

bool __cdecl
vw_save_game_state_callback(unsigned char **buffer, int *len, int *checksum, int)
{
   *len = sizeof(gs);
   *buffer = (unsigned char *)malloc(*len);
   if (!*buffer) {
      return false;
   }
   memcpy(*buffer, &gs, *len);
   *checksum = fletcher32_checksum((short *)*buffer, *len / 2);
   return true;
}

void __cdecl 
vw_free_buffer(void *buffer)
{
   free(buffer);
}

bool __cdecl
vw_log_game_state(char *filename, unsigned char *buffer, int)
{
   return true;
}


void IntroState::VectorWar_Init(unsigned short localport, int num_players, GGPOPlayer *players, int num_spectators)
{
	// srand(time(0)); 
	GGPOErrorCode result;

	gs.init();
	ngs.num_players = num_players;

	GGPOSessionCallbacks cb = {0};
	cb.begin_game      = vw_begin_game_callback;
   cb.advance_frame	 = vw_advance_frame_callback;
   cb.load_game_state = vw_load_game_state_callback;
   cb.save_game_state = vw_save_game_state_callback;
   cb.free_buffer     = vw_free_buffer;
   cb.on_event        = vw_on_event_callback;
   cb.log_game_state  = vw_log_game_state;

   #if defined(SYNC_TEST)
	   result = ggpo_start_synctest(&ggpo, &cb, "vectorwar", num_players, sizeof(int), 1);
	#else
	   result = ggpo_start_session(&ggpo, &cb, "vectorwar", num_players, sizeof(int), localport);
	#endif

   ggpo_set_disconnect_timeout(ggpo, 3000);
   ggpo_set_disconnect_notify_start(ggpo, 1000);

   int i;
   std::cout << (num_players + num_spectators) << std::endl;
   for (i = 0; i < num_players + num_spectators; i++) {
      GGPOPlayerHandle handle;
      result = ggpo_add_player(ggpo, players + i, &handle);
      ngs.players[i].handle = handle;
      ngs.players[i].type = players[i].type;
      if (players[i].type == GGPO_PLAYERTYPE_LOCAL) {
         ngs.players[i].connect_progress = 100;
         ngs.local_player_handle = handle;
         ngs.SetConnectState(handle, Connecting);
         ggpo_set_frame_delay(ggpo, handle, FRAME_DELAY);
      } else {
         ngs.players[i].connect_progress = 0;
      }
   }

}

void IntroState::VectorWar_InitSpectator(unsigned short localport, int num_players, char *host_ip, unsigned short host_port)
{
	
}

void IntroState::VectorWar_DrawCurrentFrame()
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

	// for (auto& solid : solids){
	// 	solid.draw(batchRenderer);
	// }

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

	float percentage = 1 - (gs._chars[0].GetHealth() / 420);
	int value = (420 - 100);

	gauge->SetFrame(0);
	gauge->pos.x = 0;
	gauge->draw(batchRenderer);
	gauge->SetFrame(1);
	gauge->draw(batchRenderer);
	gauge->SetFrame(2);
	gauge->draw(batchRenderer);
	gauge->SetFrame(3);
	gauge->pos = {150, 58};
	gauge->drawclip(batchRenderer, 0, 322, gs._chars[0].GetHealth(), 20, false);
	gauge->pos = {720, 58};
	gauge->drawclip(batchRenderer, 0 , 322, gs._chars[1].GetHealth(), 20, false);
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

	gs._chars[0].draw(batchRenderer, characterRenderer, ResourceManager::GetTexture("hydepal"), ResourceManager::GetTexture("shadowpal"));

   gs._chars[1].draw(batchRenderer, characterRenderer, ResourceManager::GetTexture("hydepalp2"),ResourceManager::GetTexture("shadowpal"));

   batchRenderer->EndBatch();
	batchRenderer->Flush(); //End Character Pass

	// const Stats& stats = batchRenderer->GetStats();
    // ImGui::Text("Draw Calls: %d", stats.DrawCount);
    // ImGui::Text("Quads Rendered: %d", stats.QuadCount);


	// post->EndRender();
    // post->Render(glfwGetTime());
	
}

void IntroState::VectorWar_AdvanceFrame(int inputs[], int disconnect_flags)
{
	gs.update(inputs,disconnect_flags);

	ngs.now.framenumber = gs._framenumber;
	ngs.now.checksum = fletcher32_checksum((short *)&gs, sizeof(gs) / 2);
	if((gs._framenumber % 90) == 0){
		ngs.periodic = ngs.now;
	}

	ggpo_advance_frame(ggpo);
	
}

void IntroState::VectorWar_RunFrame()
{
	GGPOErrorCode result = GGPO_OK;
	int disconnect_flags;
	int inputs[MAX_SHIPS] = {0};

	if (ngs.local_player_handle != GGPO_INVALID_HANDLE) {
		int input = ReadInputs(ngs.local_player_handle - 1);
   #if defined(SYNC_TEST)
     	input = rand(); // test: use random inputs to demonstrate sync testing
	#endif
      result = ggpo_add_local_input(ggpo, ngs.local_player_handle, &input, sizeof(input));
   }

   if (GGPO_SUCCEEDED(result)) {
   	result = ggpo_synchronize_input(ggpo,(void*)inputs,sizeof(int)*MAX_SHIPS,&disconnect_flags);
   	if(GGPO_SUCCEEDED(result)){
   		VectorWar_AdvanceFrame(inputs,disconnect_flags);
   	}
   }

}

void IntroState::VectorWar_Idle(int time)
{
	ggpo_idle(ggpo, time);
}

void IntroState::VectorWar_DisconnectPlayer(int player)
{
	if (player < ngs.num_players) {
      char logbuf[128];
      GGPOErrorCode result = ggpo_disconnect_player(ggpo, ngs.players[player].handle);
      if (GGPO_SUCCEEDED(result)) {
         sprintf_s(logbuf, ARRAYSIZE(logbuf), "Disconnected player %d.\n", player);
      } else {
         sprintf_s(logbuf, ARRAYSIZE(logbuf), "Error while disconnecting player (err:%d).\n", result);
      }
      std::cout << logbuf << std::endl;
   }
}

void IntroState::VectorWar_Exit()
{

   if (ggpo) {
      ggpo_close_session(ggpo);
      ggpo = NULL;
   }
}



bool IntroState::exit()
{
	VectorWar_Exit();
	delete icons;

	// delete inputHandler;
	// delete inputHandler2;

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

bool IntroState::enter(const std::vector<std::string>& args)
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
    // ResourceManager::LoadTexture("res/textures/pop_cat.png", true, "popcat");
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
	icons = new Spritesheet(ResourceManager::GetTexture("icons-capcom-32"), "res/textures/ui/icons-capcom-32.json", 0, 0, 32, 32, 0);
	icons->SetFrame(0);

	gauge = new Spritesheet(ResourceManager::GetTexture("gauge"), "res/textures/ui/gauge_00.json", 0, 0, 1024, 1024, 0);
	gauge->SetFrame(0);
	gauge->SetScale({1.0f, 1.0f});


	cameraXPos = 0;
	cameraYPos = 180;
	cameraRot = 0.0f;
	// cameraScale = 0.6f;

	// post->Chromatic = true;

	int offset = 0, local_player = 0;
	WSADATA wd = { 0 };
   wchar_t wide_ip_buffer[128];
   unsigned int wide_ip_buffer_size = (unsigned int)ARRAYSIZE(wide_ip_buffer);

   WSAStartup(MAKEWORD(2, 2), &wd);

   std::wstring wargs[args.size()];
   for (int i = 0; i < args.size(); ++i) {
   	wargs[i] = std::wstring(args[i].begin(), args[i].end());
   }

   const wchar_t* arguments[args.size()];
   for (int i = 0; i < args.size(); ++i) {
   	arguments[i] = wargs[i].c_str();
   }

   // const wchar_t* arguments[] = {
   //      L"7000",
   //      L"2",
   //      L"local",
   //      L"127.0.0.1:7001"
  	// };

	unsigned short local_port = (unsigned short)_wtoi(arguments[offset++]);
	int num_players = _wtoi(arguments[offset++]);
	std::cout << "num_players" << num_players <<  std::endl;


	if (wcscmp(arguments[offset], L"spectate") == 0) {
      char host_ip[128];
      unsigned short host_port;
      if (swscanf_s(arguments[offset+1], L"%[^:]:%hu", wide_ip_buffer, wide_ip_buffer_size, &host_port) != 2) {
         	std::cout << "no" << std::endl;
         	return false;
      }
      wcstombs_s(nullptr, host_ip, ARRAYSIZE(host_ip), wide_ip_buffer, _TRUNCATE);
      VectorWar_InitSpectator(local_port, num_players, host_ip, host_port);
   } else {
      GGPOPlayer players[GGPO_MAX_SPECTATORS + GGPO_MAX_PLAYERS];

      int i;
      for (i = 0; i < num_players; i++) {
         const wchar_t *arg = arguments[offset++];
         wprintf(L"arg = %s\n", arg);

         players[i].size = sizeof(players[i]);
         players[i].player_num = i + 1;
         if (!_wcsicmp(arg, L"local")) {
            players[i].type = GGPO_PLAYERTYPE_LOCAL;
            local_player = i;
            continue;
         }
         
         players[i].type = GGPO_PLAYERTYPE_REMOTE;
         if (swscanf_s(arg, L"%[^:]:%hd", wide_ip_buffer, wide_ip_buffer_size, &players[i].u.remote.port) != 2) {
         	std::cout << "no" << std::endl;
         	return false;
         }
         wcstombs_s(nullptr, players[i].u.remote.ip_address, ARRAYSIZE(players[i].u.remote.ip_address), wide_ip_buffer, _TRUNCATE);
      }
      // these are spectators...
      int num_spectators = 0;
      while (offset < __argc) {
         players[i].type = GGPO_PLAYERTYPE_SPECTATOR;
         if (swscanf_s(arguments[offset++], L"%[^:]:%hd", wide_ip_buffer, wide_ip_buffer_size, &players[i].u.remote.port) != 2) {
         	std::cout << "no" << std::endl;
         	return false;
         }
         wcstombs_s(nullptr, players[i].u.remote.ip_address, ARRAYSIZE(players[i].u.remote.ip_address), wide_ip_buffer, _TRUNCATE);
         i++;
         num_spectators++;
      }

      VectorWar_Init(local_port, num_players, players, num_spectators);
   }
	// gs.init();

	return success;
}

int IntroState::ReadInputs(int _charID)
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
		if(gs._chars[_charID].GetFlipped())
			inputs |= ButtonIDs::BACK;
		else
			inputs |= ButtonIDs::FORWARD;
	}
	if (Keys[GLFW_KEY_A])
	{
		if(gs._chars[_charID].GetFlipped())
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
			if(gs._chars[0].GetFlipped())
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
			if(gs._chars[0].GetFlipped())
				inputs |= ButtonIDs::FORWARD;
			else
				inputs |= ButtonIDs::BACK;
		}

		// if(Keys[GLFW_KEY_R])
		// {
		// 	gs.testChar->pos.x = 1500;
		// 	gs.testChar2->pos.x = 2500;
		// }

	}
	return inputs;
}

void IntroState::network(int ms)
{
	VectorWar_Idle(ms);
}

void IntroState::update(float dt)
{

	if(this->Keys[GLFW_KEY_LEFT])
	{
		cameraXPos -= 1;
	}
	if(this->Keys[GLFW_KEY_RIGHT])
	{
		cameraXPos += 1;
	}
	if(this->Keys[GLFW_KEY_UP])
	{
		cameraYPos -= 5;
	}
	if(this->Keys[GLFW_KEY_DOWN])
	{
		cameraYPos += 5;
	}
	if (this->Keys[GLFW_KEY_Z]){
		cameraScale -= .01;
	}
	if (this->Keys[GLFW_KEY_X]){
		cameraScale += .01;
	}

	float frequency = 0.40f;

	shake = (gs.trauma * gs.trauma);
	offsetX = maxOffset.x * shake * perlin.noise1D(tick * frequency);
	offsetY = maxOffset.y * shake * perlin.noise1D((tick + 1) * frequency);
	angle = maxAngle * shake * perlin.noise1D((tick + 2) * frequency);

	// std::vector<int> inputNumbers;

	// std::bitset<7> newButtonState(inputHandler->butPress.to_ullong());
	// if((newButtonState & FK_Input_Buttons.LP) != 0) inputNumbers.push_back(8);
	// if((newButtonState & FK_Input_Buttons.MP) != 0) inputNumbers.push_back(9);
	// if((newButtonState & FK_Input_Buttons.HP) != 0) inputNumbers.push_back(10);
	// if((newButtonState & FK_Input_Buttons.LK) != 0) inputNumbers.push_back(11);
	// if((newButtonState & FK_Input_Buttons.MK) != 0) inputNumbers.push_back(12);
	// if((newButtonState & FK_Input_Buttons.HK) != 0) inputNumbers.push_back(13);

	// if(inputHandler->inputChanged){
	// 	if (inputHandler->currentDirection == "UP") {
	// 	    inputNumbers.push_back(2);
	// 	} if (inputHandler->currentDirection == "BACK") {
	// 	    inputNumbers.push_back(0);
	// 	} if (inputHandler->currentDirection == "FORWARD") {
	// 		inputNumbers.push_back(1);
	// 	} if (inputHandler->currentDirection == "DOWN") {
	// 	    inputNumbers.push_back(3);
	// 	} if (inputHandler->currentDirection == "UP_FORWARD") {
	// 	    inputNumbers.push_back(5);
	// 	} if (inputHandler->currentDirection == "UP_BACK") {
	// 	    inputNumbers.push_back(4);
	// 	} if (inputHandler->currentDirection == "DOWN_BACK") {
	// 	    inputNumbers.push_back(6);
	// 	} if (inputHandler->currentDirection == "DOWN_FORWARD") {
	// 	    inputNumbers.push_back(7);
	// 	}
	// }

	// if (!inputNumbers.empty()) {
	//     currentIndex = (currentIndex + 1) % MAX_HISTORY_SIZE;
	    
	//     inputHistory[currentIndex] = inputNumbers;
	    
	//     if (inputHistory.size() > MAX_HISTORY_SIZE) {
	//         int oldestIndex = (currentIndex + 1) % MAX_HISTORY_SIZE;
	//         inputHistory.erase(oldestIndex);
	//     }
	// }

	// if(!Keys[GLFW_KEY_LEFT_CONTROL] && (!Keys[GLFW_KEY_S] || !Keys[GLFW_KEY_L]))
	// {
	// 	savingLoading = false;
	// }

	// if(!savingLoading && Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_S]){
	// 	vw_save_game_state_callback(&buffer, &len, &checksum, 0);
	// 	std::cout << "saved" << std::endl;
	// 	// std::cout << checksum << std::endl;
	// 	savingLoading = true;
	// }

	// if(!savingLoading && Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_L]){
	// 	vw_load_game_state_callback(buffer, len);
	// 	savingLoading = true;
	// }

	VectorWar_RunFrame();
	// gs.update(inputs,0);

	float distance = gs._chars[1].GetCenterPos().x - gs._chars[0].GetCenterPos().x;
	float distanceY = gs._chars[1].GetCenterPos().y - gs._chars[0].GetCenterPos().y;

	cameraXPos += (abs(gs._chars[1].GetCenterPos().x - (distance)/2) - cameraXPos) * cameraSnap;
	cameraYPos += (abs(gs._chars[1].GetCenterPos().y - (distanceY)/2) - cameraYPos) * cameraSnap;

	if(cameraXPos <= cameraMinPos)
		cameraXPos = cameraMinPos;

	if(cameraXPos >= cameraMaxPos)
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
	VectorWar_DrawCurrentFrame();
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