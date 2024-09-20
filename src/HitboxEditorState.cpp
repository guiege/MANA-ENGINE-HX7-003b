#include "HitboxEditorState.h"

Renderer* spriteRenderer;
Renderer* shadowRenderer;
Renderer* paletteRenderer;
Camera* m_Camera;
PostProcessor* post;

bool usePalette = false;

int frameBruh = 0;

float skew = 0.0f;

float shadowIntensity = 1.0f;
int spritesheetFrame = 0;
bool frameFlipped = false;
int fadeInTick = 10;

//CAMERA CONTROLS
float cameraXPos = 0;
float cameraYPos = 0;
float cameraRot = 0.0f;
float cameraScale = 1.0f;
double lastX = 0.0, lastY = 0.0;
bool isPanning = false;

glm::vec2 segmentA = glm::vec2(960, 700); // Bottom Left
glm::vec2 segmentB = glm::vec2(960, 520); // Top Left
glm::vec2 segmentC = glm::vec2(1500, 520); // Top Right
glm::vec2 segmentD = glm::vec2(1500, 700); // Bottom Right

//HITBOX EDITOR
rect curHurtbox = rect{0, 0, 0, 0};
std::map<int, std::vector<rect>> hurtboxes;
bool drawingHurtbox = false;
int selectedHurtbox = -1;
int edgeThreshold = 3;
bool hurtboxOnionSkinning = false;
int resizeKeyCooldown = 0;
int resizeSpeed = 4;
glm::vec3 hurtboxColor = {10/(float)255, 185/(float)255, 230/(float)255};
glm::vec3 hurtboxSecondaryColor = {150/(float)255, 245/(float)255, 245/(float)255};
glm::vec3 hitboxColor = {1, 60/(float)255, 40/(float)255};
glm::vec3 hitboxSecondaryColor = {1, 175/(float)255, 175/(float)255};
glm::vec3 pushboxColor = {30/(float)255, 220/(float)255, 0};
glm::vec3 pushboxSecondaryColor = {130/(float)255, 1, 150/(float)255};
int vertCrossWidth = 2;
int vertCrossHeight = 40;
int horiCrossWidth = 32;
int horiCrossHeight = 1;
bool writingBoxes = false;
bool loadingBoxes = false;
bool copyingBoxes = false;
bool movingBox = false;
bool deletingBox = false;
std::string boxesPath = "scripts/hitboxes/testBoxes.hbox";
std::string loadBoxesPath = "scripts/hitboxes/testBoxes.hbox";
static bool open_clear_warning = false;
static bool open_save_warning = false;
int curHitboxType = 0;

Spritesheet* testSheet;

bool HitboxEditorState::exit()
{

	spriteRenderer->Delete();
	delete spriteRenderer;

	shadowRenderer->Delete();
	delete shadowRenderer;

	paletteRenderer->Delete();
	delete paletteRenderer;

	batchRenderer->Delete();
	delete batchRenderer;

	post->Delete();
	delete post;

	return true;
}

bool HitboxEditorState::enter()
{
	bool success = true;

	m_Camera = new Camera(0, 1920, 1080, 0);

	usePalette = false;
	frameBruh = 0;
	spritesheetFrame = 0;
	skew = 0;
	shadowIntensity = 1.0f;
	cameraXPos = 0;
	cameraYPos = 0;
	cameraRot = 0.0f;
	cameraScale = 1.0f;

	ResourceManager::LoadShader("res/shaders/default.vert", "res/shaders/default.frag", "sprite");
	ResourceManager::LoadShader("res/shaders/default.vert", "res/shaders/shadow.frag", "shadow");
	ResourceManager::LoadShader("res/shaders/post.vert", "res/shaders/post.frag", "post");
	ResourceManager::LoadShader("res/shaders/default.vert", "res/shaders/palette.frag", "palette");
    ResourceManager::LoadShader("res/shaders/batch.vert", "res/shaders/batch.frag", "batch");

	ResourceManager::LoadTexture("res/textures/hydesheet.png", true, "hydesheet");
	// ResourceManager::LoadTexture("res/textures/Arcueid/000.png", true, "arc");
	// ResourceManager::LoadTexture("res/textures/arcpal.png", true, "arcpal");
	// ResourceManager::LoadTexture("res/textures/guiegecustompal.png", true, "guiegepal");

	glm::mat4 proj = glm::ortho(0.0f, (float)(1920), (float)(1080), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", proj);
    ResourceManager::GetShader("palette").Use().SetInteger("image", 0);
    ResourceManager::GetShader("palette").SetMatrix4("projection", proj);
    ResourceManager::GetShader("shadow").Use().SetInteger("image", 0);
    ResourceManager::GetShader("shadow").SetMatrix4("projection", proj);
    ResourceManager::GetShader("post").Use().SetMatrix4("projection", proj);

    // for (int i = 0; i < 16; i++) {
    //     std::string name = "";
    //     std::string id = "res/textures/Arcueid/";

    //     std::string numStr = std::to_string(i);
    //     if (numStr.length() == 1) {
    //         id.append("00" + numStr);
    //     } else if (numStr.length() == 2) {
    //         id.append("0" + numStr);
    //     } else {
    //         id.append(numStr);
    //     }
    //     id.append(".bmp");
    //     name.append(numStr);
    // 	ResourceManager::LoadTexture(id.c_str(), true, name.c_str());

    // 	std::cout << id << std::endl;
    // }

    testSheet = new Spritesheet(ResourceManager::GetTexture("hydesheet"), "res/textures/hydesheet.json", 0, 0, 4296, 15673, 0);

    post = new PostProcessor(ResourceManager::GetShader("post"), 1920, 1080);

	spriteRenderer = new Renderer(ResourceManager::GetShader("sprite"));
	shadowRenderer = new Renderer(ResourceManager::GetShader("shadow"));
	paletteRenderer = new Renderer(ResourceManager::GetShader("palette"));
	batchRenderer = new Renderer(ResourceManager::GetShader("batch"));
	batchRenderer->initBatchRenderData();

	return success;
}

void HitboxEditorState::update(float dt)
{

	if(this->Keys[GLFW_KEY_I]){
		usePalette = true;
	}
	if(this->Keys[GLFW_KEY_O]){
		usePalette = false;
	}

	// if(this->Keys[GLFW_KEY_J]){
	// 	post->lightsOut = true;
	// }
	// if(this->Keys[GLFW_KEY_K]){
	// 	post->lightsOut = false;
	// }


	if(this->Keys[GLFW_KEY_N] && !frameFlipped){
		spritesheetFrame--;
		selectedHurtbox = -1;
		frameFlipped = true;
	}
	if(this->Keys[GLFW_KEY_M] && !frameFlipped){
		spritesheetFrame++;
		selectedHurtbox = -1;
		frameFlipped = true;
	}

	if(!Keys[GLFW_KEY_M] && !Keys[GLFW_KEY_N])
		frameFlipped = false;

	if (this->Keys[GLFW_KEY_Z]){
		cameraScale -= .1;
	}
	if (this->Keys[GLFW_KEY_X]){
		cameraScale += .1;
	}

	if(Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_L] && !loadingBoxes){
		loadingBoxes = true;
		hurtboxes.clear();
		readHurtboxesFromFile(hurtboxes, loadBoxesPath);
	}
	if(!Keys[GLFW_KEY_LEFT_CONTROL] && !Keys[GLFW_KEY_L])
		loadingBoxes = false;

	if (Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_C] && !copyingBoxes) {
	    copyingBoxes = true;

	    for (const rect& box : hurtboxes[spritesheetFrame - 1]) {
	        if (box.type == curHitboxType) {
	            hurtboxes[spritesheetFrame].push_back(box);
	        }
	    }
	}

	if(!Keys[GLFW_KEY_LEFT_CONTROL] && !Keys[GLFW_KEY_C])
		copyingBoxes = false;

	if(frameBruh > 15)
		frameBruh = 0;
	if(frameBruh < 0)
		frameBruh = 15;

	if(spritesheetFrame > testSheet->getLength()){
		spritesheetFrame = 0;
	}
	if(spritesheetFrame < 0){
		spritesheetFrame = testSheet->getLength();
	}


	post->fadeFactor = std::max(0.0f, std::min((static_cast<float>(tick) / fadeInTick), 1.0f));

	// post->lightPosX = mouseX;
	// post->lightPosY = mouseY;

	testSheet->SetFrame(spritesheetFrame);
	

}

void HitboxEditorState::render()
{
	m_Camera->SetPosition(glm::vec3(cameraXPos, cameraYPos, 0));
	m_Camera->SetRotation(cameraRot);
	m_Camera->SetScale(cameraScale);

	glm::vec2 centerOffset = glm::vec2(0.5f * 1920, 0.5f * 1080);
	glm::vec2 cursorPosRelativeToCenter = glm::vec2(mouseX, mouseY) - centerOffset;
	glm::vec2 scaledCursorPos = cursorPosRelativeToCenter * cameraScale;
	glm::vec2 worldCursorPos = scaledCursorPos + centerOffset + glm::vec2(cameraXPos, cameraYPos);

	if (!movingBox && hurtboxes.count(spritesheetFrame) > 0) {
	    std::vector<rect>& rects = hurtboxes[spritesheetFrame];

	    for (size_t i = 0; i < rects.size(); ++i) {
	    	selectedHurtbox = -1;
	        rect& r = rects[i];
	        if (r.checkCollision(worldCursorPos)) {
	            selectedHurtbox = static_cast<int>(i);
	            break;
	        }
	    }
	}

	if (selectedHurtbox != -1){
		if(!deletingBox && Keys[GLFW_KEY_DELETE]){
			hurtboxes[spritesheetFrame].erase(hurtboxes[spritesheetFrame].begin() + selectedHurtbox);
			deletingBox = true;
			selectedHurtbox = -1;
		}

		if(!Keys[GLFW_KEY_DELETE])
			deletingBox = false;

		if (resizeKeyCooldown < tick) {
			if(Keys[GLFW_KEY_DOWN]){
				resizeKeyCooldown = tick + resizeSpeed;
				if(Keys[GLFW_KEY_LEFT_SHIFT]){
					hurtboxes[spritesheetFrame][selectedHurtbox].y++;
					hurtboxes[spritesheetFrame][selectedHurtbox].height--;
				}
				else
					hurtboxes[spritesheetFrame][selectedHurtbox].height++;
			}
			if(Keys[GLFW_KEY_UP]){
				resizeKeyCooldown = tick + resizeSpeed;
				if(Keys[GLFW_KEY_LEFT_SHIFT]){
					hurtboxes[spritesheetFrame][selectedHurtbox].y--;
					hurtboxes[spritesheetFrame][selectedHurtbox].height++;
				}
				else{
					hurtboxes[spritesheetFrame][selectedHurtbox].height--;
				}
			}

			if(Keys[GLFW_KEY_LEFT_SHIFT]){
				if(Keys[GLFW_KEY_RIGHT]){
					resizeKeyCooldown = tick + resizeSpeed;
					hurtboxes[spritesheetFrame][selectedHurtbox].width++;
				}

				if(Keys[GLFW_KEY_LEFT]){
					resizeKeyCooldown = tick + resizeSpeed;
					hurtboxes[spritesheetFrame][selectedHurtbox].width--;
				}

			}else{
				if(Keys[GLFW_KEY_LEFT]){
					resizeKeyCooldown = tick + resizeSpeed;
					hurtboxes[spritesheetFrame][selectedHurtbox].width++;
					hurtboxes[spritesheetFrame][selectedHurtbox].x--;
				}

				if(Keys[GLFW_KEY_RIGHT]){
					resizeKeyCooldown = tick + resizeSpeed;
					hurtboxes[spritesheetFrame][selectedHurtbox].width--;
					hurtboxes[spritesheetFrame][selectedHurtbox].x++;
				}
			}
		}
	}

	movingBox = false;

	if(leftMouse){
		if(selectedHurtbox == -1){
			if(!drawingHurtbox){
				drawingHurtbox = true;
				curHurtbox.x = (int)(worldCursorPos.x);
				curHurtbox.y = (int)(worldCursorPos.y);
				curHurtbox.width = 0;
				curHurtbox.height = 0;
			}
		} else{
			if (hurtboxes.count(spritesheetFrame) == 0) {
				hurtboxes[spritesheetFrame] = std::vector<rect>();
			}
			if (!drawingHurtbox && selectedHurtbox != -1 && static_cast<size_t>(selectedHurtbox) < hurtboxes[spritesheetFrame].size()) {
				movingBox = true;
			    rect& selectedBox = hurtboxes[spritesheetFrame][selectedHurtbox];
			    selectedBox.x = (int)(worldCursorPos.x - (selectedBox.width / 2));
			    selectedBox.y = (int)(worldCursorPos.y - (selectedBox.height / 2));
			}
		}
	}else{
	    if(mouseX < 1450 && !open_save_warning && drawingHurtbox)
	    {
			curHurtbox.width = abs((int)(worldCursorPos.x) - curHurtbox.x) + 1;
			curHurtbox.height = abs((int)(worldCursorPos.y) - curHurtbox.y) + 1;

			if ((int)(worldCursorPos.x) - curHurtbox.x < 0) {
			    curHurtbox.x = (int)(worldCursorPos.x);
			}

			if ((int)(worldCursorPos.y) - curHurtbox.y < 0) {
			    curHurtbox.y = (int)(worldCursorPos.y);
			}

			curHurtbox.type = curHitboxType;

			if (hurtboxes.count(spritesheetFrame) == 0) {
				hurtboxes[spritesheetFrame] = std::vector<rect>();
			}

			hurtboxes[spritesheetFrame].push_back(curHurtbox);
			sortAllBoxesInFrames(hurtboxes);
	    }
	    drawingHurtbox = false;
	}

	if(middleMouse){
		if(!isPanning){
		lastX = mouseX;
		lastY = mouseY;
		}
		isPanning = true;
	}
	else{
		isPanning = false;
	}


	if (isPanning) {
        double xOffset = lastX - mouseX;
        double yOffset = lastY - mouseY;

        cameraXPos += xOffset * cameraScale;
        cameraYPos += yOffset * cameraScale;
    }

    lastX = mouseX;
    lastY = mouseY;

	ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());
	ResourceManager::GetShader("palette").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());
	ResourceManager::GetShader("shadow").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());
	ResourceManager::GetShader("batch").Use().SetMatrix4("projection", m_Camera->GetViewProjectionMatrix());

	// post->BeginRender();

	batchRenderer->ResetStats();
	batchRenderer->BeginBatch();

	testSheet->draw(batchRenderer);

	if(curHitboxType == 0)
		batchRenderer->DrawQuad({(int)(worldCursorPos.x), (int)(worldCursorPos.y)}, {1, 1}, 0, glm::vec4(hurtboxSecondaryColor, 1.0f));
	else if(curHitboxType == 1)
		batchRenderer->DrawQuad({(int)(worldCursorPos.x), (int)(worldCursorPos.y)}, {1, 1}, 0, glm::vec4(hitboxSecondaryColor, 1.0f));
	else if(curHitboxType == 2)
		batchRenderer->DrawQuad({(int)(worldCursorPos.x), (int)(worldCursorPos.y)}, {1, 1}, 0, glm::vec4(pushboxSecondaryColor, 1.0f));

	if(drawingHurtbox){
		if(curHitboxType == 0)
			batchRenderer->DrawOutline({curHurtbox.x, curHurtbox.y}, {(int)(worldCursorPos.x) - curHurtbox.x + 1, (int)(worldCursorPos.y) - curHurtbox.y + 1}, 0, glm::vec4(hurtboxSecondaryColor, 1.0f), 1);
		else if(curHitboxType == 1)
			batchRenderer->DrawOutline({curHurtbox.x, curHurtbox.y}, {(int)(worldCursorPos.x) - curHurtbox.x + 1, (int)(worldCursorPos.y) - curHurtbox.y + 1}, 0, glm::vec4(hitboxSecondaryColor, 1.0f), 1);
		else if(curHitboxType == 2)
			batchRenderer->DrawOutline({curHurtbox.x, curHurtbox.y}, {(int)(worldCursorPos.x) - curHurtbox.x + 1, (int)(worldCursorPos.y) - curHurtbox.y + 1}, 0, glm::vec4(pushboxSecondaryColor, 1.0f), 1);
	}

	if(hurtboxes.count(spritesheetFrame) > 0){
		for (int i = 0; i < hurtboxes[spritesheetFrame].size(); ++i){
			if(i == selectedHurtbox){
				if(hurtboxes[spritesheetFrame][i].type == 0)
					batchRenderer->DrawQuad({hurtboxes[spritesheetFrame][i].x, hurtboxes[spritesheetFrame][i].y}, {hurtboxes[spritesheetFrame][i].width, hurtboxes[spritesheetFrame][i].height}, 0, glm::vec4(hurtboxColor, 0.3f));
				else if(hurtboxes[spritesheetFrame][i].type == 1)
					batchRenderer->DrawQuad({hurtboxes[spritesheetFrame][i].x, hurtboxes[spritesheetFrame][i].y}, {hurtboxes[spritesheetFrame][i].width, hurtboxes[spritesheetFrame][i].height}, 0, glm::vec4(hitboxColor, 0.3f));
				else if(hurtboxes[spritesheetFrame][i].type == 2)
					batchRenderer->DrawQuad({hurtboxes[spritesheetFrame][i].x, hurtboxes[spritesheetFrame][i].y}, {hurtboxes[spritesheetFrame][i].width, hurtboxes[spritesheetFrame][i].height}, 0, glm::vec4(pushboxColor, 0.3f));
			}

			if(hurtboxes[spritesheetFrame][i].type == 0)
				batchRenderer->DrawOutline({hurtboxes[spritesheetFrame][i].x, hurtboxes[spritesheetFrame][i].y}, {hurtboxes[spritesheetFrame][i].width, hurtboxes[spritesheetFrame][i].height}, 0, glm::vec4(hurtboxColor, 1.0f), 1);
			else if(hurtboxes[spritesheetFrame][i].type == 1)
				batchRenderer->DrawOutline({hurtboxes[spritesheetFrame][i].x, hurtboxes[spritesheetFrame][i].y}, {hurtboxes[spritesheetFrame][i].width, hurtboxes[spritesheetFrame][i].height}, 0, glm::vec4(hitboxColor, 1.0f), 1);
			else if(hurtboxes[spritesheetFrame][i].type == 2){
				glm::vec2 pos = {hurtboxes[spritesheetFrame][i].x, hurtboxes[spritesheetFrame][i].y};
				batchRenderer->DrawOutline(pos, {hurtboxes[spritesheetFrame][i].width, hurtboxes[spritesheetFrame][i].height}, 0, glm::vec4(pushboxColor, 1.0f), 1);
				batchRenderer->DrawQuad(pos + glm::vec2(hurtboxes[spritesheetFrame][i].width/2 - vertCrossWidth/2, 
										hurtboxes[spritesheetFrame][i].height - vertCrossHeight/2), glm::vec2(vertCrossWidth, vertCrossHeight), 0, glm::vec4(1.0f));
				batchRenderer->DrawQuad(pos + glm::vec2(hurtboxes[spritesheetFrame][i].width/2 - horiCrossWidth/2,
										 hurtboxes[spritesheetFrame][i].height - horiCrossHeight/2), glm::vec2(horiCrossWidth, horiCrossHeight), 0, glm::vec4(1.0f));
			}
		}
	}

	if(hurtboxOnionSkinning && hurtboxes.count(spritesheetFrame - 1) > 0){
		for(const rect& r : hurtboxes.at(spritesheetFrame - 1)){
			if(r.type == 0)
				batchRenderer->DrawOutline({r.x, r.y}, {r.width, r.height}, 0, glm::vec4(hurtboxSecondaryColor, 0.2f), 1);
			else if(r.type == 1)
				batchRenderer->DrawOutline({r.x, r.y}, {r.width, r.height}, 0, glm::vec4(hitboxSecondaryColor, 0.2f), 1);
			else if(r.type == 2)
				batchRenderer->DrawOutline({r.x, r.y}, {r.width, r.height}, 0, glm::vec4(pushboxSecondaryColor, 0.2f), 1);
		}
	}

	ImGui::InputInt("spritesheetFrame", &spritesheetFrame);
	ImGui::InputInt("resizeSpeed", &resizeSpeed);
	ImGui::InputInt("curHitboxType", &curHitboxType);

	char buffer[256];
	strcpy(buffer, boxesPath.c_str());
	if (ImGui::InputText("Save to Filepath", buffer, IM_ARRAYSIZE(buffer))) 
		boxesPath = std::string(buffer);

	char buff2er[256];
	strcpy(buff2er, loadBoxesPath.c_str());
	if (ImGui::InputText("Load from Filepath", buff2er, IM_ARRAYSIZE(buff2er))) 
		loadBoxesPath = std::string(buff2er);

	if (ImGui::Button("Clear Hurtboxes")) {
	    open_clear_warning = true;
	}

	if(ImGui::Button("Sort Hurtboxes")) {
		sortAllBoxesInFrames(hurtboxes);
	}

	if(Keys[GLFW_KEY_LEFT_CONTROL] && Keys[GLFW_KEY_S] && !writingBoxes){
		open_save_warning = true;
	}

	if(open_save_warning){
	    ImGui::OpenPopup("Save Boxes Warning!!!");
	    if (ImGui::BeginPopupModal("Save Boxes Warning!!!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
	        ImGui::Text("You really wanna save those boxes?\nTake your time and make sure you've saved.");
	        ImGui::Spacing();

	        if (ImGui::Button("yeah go ahead")) {
	        	writingBoxes = true;
				writeHurtboxesToFile(hurtboxes, boxesPath);
	            ImGui::CloseCurrentPopup();
	            open_save_warning = false;
	            writingBoxes = false;
	        }
	        ImGui::SameLine();
	        if (ImGui::Button("nah")) {
	            ImGui::CloseCurrentPopup();
	            open_save_warning = false;
	            writingBoxes = false;
	        }

	        ImGui::EndPopup();
	    }
	}

	if (open_clear_warning) {
	    ImGui::OpenPopup("Clear Hurtboxes Warning!!!");
	    if (ImGui::BeginPopupModal("Clear Hurtboxes Warning!!!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
	        ImGui::Text("You really wanna clear those hurtboxes?\nTake your time and make sure you've saved.");
	        ImGui::Spacing();

	        if (ImGui::Button("yeah go ahead")) {
	            hurtboxes.clear();
	            selectedHurtbox = -1;
	            ImGui::CloseCurrentPopup();
	            open_clear_warning = false;
	        }
	        ImGui::SameLine();
	        if (ImGui::Button("nah")) {
	            ImGui::CloseCurrentPopup();
	            open_clear_warning = false;
	        }

	        ImGui::EndPopup();
	    }
	}


	ImGui::Checkbox("hurtboxOnionSkinning", &hurtboxOnionSkinning);

	batchRenderer->EndBatch();
	batchRenderer->Flush();

	// post->EndRender();
    // post->Render(glfwGetTime());
}

HitboxEditorState HitboxEditorState::sHitboxEditorState;

HitboxEditorState::HitboxEditorState()
{

}

HitboxEditorState* HitboxEditorState::get()
{
	return &sHitboxEditorState;
}


//OLD SHIT
	// int ypos = 667;
	// int bottompos = (ypos +(ypos /2));

	// shadowRenderer->DrawTexture(ResourceManager::GetTexture(std::to_string(frameBruh)), 0, ypos, 960, ypos, 960 + (100 * skew), bottompos, (100 * skew), bottompos, 0.12, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));

	// if(usePalette)
	// 	paletteRenderer->DrawIndexedTexture(ResourceManager::GetTexture(std::to_string(frameBruh)), ResourceManager::GetTexture("guiegepal"),glm::vec2(0, 0), glm::vec2(960, 768), 0, glm::vec4(1.0, 1.0, 1.0, 1.0));
	// else
	// 	paletteRenderer->DrawIndexedTexture(ResourceManager::GetTexture(std::to_string(frameBruh)), ResourceManager::GetTexture("arcpal"),glm::vec2(0, 0), glm::vec2(960, 768), 0, glm::vec4(1.0, 1.0, 1.0, 1.0));

	// spriteRenderer->DrawTexture(ResourceManager::GetTexture("hydesheet"),glm::vec2(0, 0), glm::vec2(4296, 15673), 0, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// spriteRenderer->DrawRect(glm::vec2(100, 100), glm::vec2(100, 100), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// spriteRenderer->DrawRect(glm::vec2(960, ypos), glm::vec2(10, 10), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// spriteRenderer->DrawRect(glm::vec2(960 + (100 * skew), bottompos), glm::vec2(10, 10), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// spriteRenderer->DrawRect(glm::vec2((100 * skew), bottompos), glm::vec2(10, 10), 0, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// post->ShadowPass();
	// spriteRenderer->DrawShadow(segmentA, segmentB, glm::vec2(mouseX, mouseY), glm::vec4(0.0, 0.0, 0.0, 1.0));
	// spriteRenderer->DrawShadow(segmentB, segmentC, glm::vec2(mouseX, mouseY), glm::vec4(0.0, 0.0, 0.0, 1.0));
	// spriteRenderer->DrawShadow(segmentC, segmentD, glm::vec2(mouseX, mouseY), glm::vec4(0.0, 0.0, 0.0, 1.0));
	// spriteRenderer->DrawShadow(segmentD, segmentA, glm::vec2(mouseX, mouseY), glm::vec4(0.0, 0.0, 0.0, 1.0));
