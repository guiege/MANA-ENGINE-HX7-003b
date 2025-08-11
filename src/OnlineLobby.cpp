#include "OnlineLobby.h"

const char* items[] = { "1", "2"};
static const char* current_item = NULL;
	
bool OnlineLobby::exit()
{
	FMOD_System_Release(system);
	FMOD_Channel_Stop(channel);
	return true;
}

bool OnlineLobby::enter(const std::vector<std::string>& args)
{
	bool success = true;
	desiredState = 0;
	this->args.clear();

	FMOD_RESULT result=FMOD_System_Create(&system, FMOD_VERSION);
	FMOD_System_Set3DSettings(system, 1.0,1.0f,1.0f);
	result=FMOD_System_Init(system, 512,FMOD_INIT_NORMAL | FMOD_INIT_CHANNEL_LOWPASS,nullptr);
	result=FMOD_System_CreateSound(system, "res/bgm/25. Access Connect (Network Menu).flac",FMOD_DEFAULT,nullptr,&sound);
	result=FMOD_System_PlaySound(system, sound,nullptr,false,&channel);

	return success;
}

void OnlineLobby::network(int ms){}

void OnlineLobby::update(float dt)
{
	FMOD_Channel_SetLowPassGain(channel, (1.0f - (mouseY/1080)));
	
}

void OnlineLobby::render()
{
	ImGui::Begin("NETWORK");
	ImGui::SetWindowFontScale(3.0f);
	ImGui::InputText("remoteIP", remoteIP, sizeof(remoteIP));


	if (ImGui::BeginCombo("playerNumber", current_item))
	{
	    for (int n = 0; n < IM_ARRAYSIZE(items); n++)
	    {
	        bool is_selected = (current_item == items[n]);
	        if (ImGui::Selectable(items[n], is_selected))
	            current_item = items[n];
	        if (is_selected)
	            ImGui::SetItemDefaultFocus();
	    }
	    ImGui::EndCombo();
	}
	ImGui::SetWindowFontScale(1.0f);
	ImGui::End();
	ImGui::Begin("NETWORK2");
	ImGui::SetWindowFontScale(5.0f);
	if (ImGui::Button("LET'S GO"))
	{
		if(strlen(remoteIP) != 0){
			if(current_item == "1"){
				args.push_back("7000");
				args.push_back("2");
				args.push_back("local");
				args.push_back((std::string(remoteIP) + ":7001"));
				desiredState = 1;
			} else if(current_item == "2"){
				args.push_back("7001");
				args.push_back("2");
				args.push_back((std::string(remoteIP) + ":7000"));
				args.push_back("local");
				desiredState = 1;
			}
		}
	}
	ImGui::SetWindowFontScale(1.0f);
	ImGui::End();
}


OnlineLobby OnlineLobby::sOnlineLobby;

OnlineLobby::OnlineLobby()
{

}

OnlineLobby* OnlineLobby::get()
{
	return &sOnlineLobby;
}
