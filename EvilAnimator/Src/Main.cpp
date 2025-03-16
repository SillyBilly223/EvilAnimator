
#include <raylib.h>

#include "Utils/Rl_ImGui/imgui.h"
#include "Utils/Rl_ImGui/rlImGui.h"
#include "Utils/TinyFd/tinyfiledialogs.h"

#include "Animation/AnimationScene.h"
#include "Animation/AnimationSettings.h"

//#include "Utils/FileHandling.h"

#include <string>
#include <codecvt>

AnimationScene* CurrentScene = new AnimationScene();
ANIMEDITOR_Settings Settings;

std::string WideToUTF8(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

AnimFrame RL_Frame_CreateBlank(Texture2D tex) {
	AnimFrame newframe = {
		{ 0,0,(float)tex.width,(float)tex.height },
	    { 0,0,1,1 },
		{ 0.5f,0.5f },
		0,
		{ 255, 255, 255, 255 },
		1,
		SPRTRUE
	};
	return newframe;
}

void DrawSpriteDisplay() {
	if (!CurrentScene->CanDisplayAnim()) {
		CurrentScene->AnimPlay.Playing = false;
		return;
	}

	CurrentScene->AnimPlay.TickAnimation();

	float MiddlePoint_X = GetRenderWidth() / 4;
	float MiddlePoint_Y = GetRenderHeight() / 4;

	DrawLineEx({ MiddlePoint_X,0 }, { MiddlePoint_X,(float)GetRenderHeight() }, 2.5f, DARKGRAY);
	DrawLineEx({ 0,MiddlePoint_Y }, { (float)GetRenderWidth(), MiddlePoint_Y }, 2.5f, DARKGRAY);

	DrawRectangleLinesEx({ MiddlePoint_X, MiddlePoint_Y, 32, 32 }, 2, DARKGRAY);

	if (CurrentScene->AnimPlay.OnionSkin) CurrentScene->AnimPlay.DrawOnionFrames(CurrentScene->CurrentData->Texture, { MiddlePoint_X,MiddlePoint_Y });
	CurrentScene->AnimPlay.DrawAnimationFrame(CurrentScene->CurrentData->Texture, { MiddlePoint_X,MiddlePoint_Y });
	CurrentScene->UpdateSpriteHandling(GetMousePosition(), { MiddlePoint_X,MiddlePoint_Y });
}

/*
void DrawInfoZone_LoadAnimationFile() {
	const char* filter[] = { "*anim" };
	const char* fpath = tinyfd_openFileDialog("Load Animation", "", 1, filter, "Animation (*anim)", 0);
	if (!fpath) return;
	
	FILE* file;
	if (fopen_s(&file, fpath, "rb") != 0) {
		tinyfd_messageBox("EvilAnimator", "Invalid file or path", "ok", "error", 1);
		return;
	}

	const char* errorsmg = "";

	AnimSaveData* data = new AnimSaveData();
	data->FrameCount = 0; data->Frames = nullptr; data->Image = { };

	if (!fread_s(data, sizeof(AnimSaveData), sizeof(AnimSaveData), 1, file)) 
	{ errorsmg = "ANIMSAVEDATA"; goto FAILLOADFILE; }

	if (!data->FrameCount) { errorsmg = "FAILEDLOAD/NOFRAMES - FRAMECOUNT"; goto FAILLOADFILE; }

	data->Frames = new AnimFrame[data->FrameCount];
	if (!data->Frames) { errorsmg = "ALLOCATE_ANIMFRAME"; goto FAILLOADFILE; }

	if (fread_s(data->Frames, sizeof(AnimFrame) * data->FrameCount, sizeof(AnimFrame), data->FrameCount, file) != data->FrameCount)
	{ errorsmg = "ANIMFRAMES"; goto FAILLOADFILE; }

	if (!fread_s(&data->Image, sizeof(SprImage), sizeof(SprImage), 1, file))
	{ errorsmg = "ANIMIMAGEINFO"; goto FAILLOADFILE; }

	data->Image.data = malloc(data->Image.width * data->Image.height * 4);
	if (!data->Image.data) { errorsmg = "ALLOCATE_ANIMIMAGEDATA"; goto FAILLOADFILE; }

	if (!fread_s(data->Image.data, data->Image.width * data->Image.height * 4, data->Image.width * data->Image.height * 4, 1, file))
	{ errorsmg = "ANIMIMAGEDATA"; goto FAILLOADFILE; }

	CurrentScene->LoadScene(*data);

	delete[] data->Frames;
	delete data;
	fclose(file);
	return;
FAILLOADFILE:
	tinyfd_messageBox("EvilAnimator", TextFormat("Failed to load Animation (%s)", errorsmg), "ok", "error", 1);
	if (data->Frames != nullptr) delete[] data->Frames;
	delete data;
	fclose(file);
}

void DrawInfoZone_SaveAnimationFile() {

	if (!CurrentScene->CurrentData->Frames.size()) {
		tinyfd_messageBox("EvilAnimator", TextFormat("Failed to save Animation (NOFRAMES)"), "ok", "error", 1);
		return;
	}

	const char* filter[] = { "*anim" };
	const char* fpath = tinyfd_saveFileDialog("Save Animation", "data.anim", 1, filter, "Animation (*anim)");
	if (!fpath) return;

	FILE* file;
	if (fopen_s(&file, fpath, "wb") != 0) {
		tinyfd_messageBox("EvilAnimator", "Failed to open file for saving", "ok", "error", 1);
		return;
	}

	const char* errorsmg = "";

	AnimSaveData data;
	CurrentScene->ConvertCurrentSceneToAnimData(&data);

	if (!data.Image.data)
	{ errorsmg = "ALLOCATE_ANIMIMAGEDATA"; goto FILEFAILSAVE; }

	if (!fwrite(&data, sizeof(AnimSaveData), 1, file))
	{ errorsmg = "ANIMSAVEDATA"; goto FILEFAILSAVE; }

	if (!fwrite(data.Frames, sizeof(AnimFrame), data.FrameCount, file))
	{ errorsmg = "ANIMFAMES"; goto FILEFAILSAVE; }

	if (!fwrite(&data.Image, sizeof(SprImage), 1, file))
	{ errorsmg = "ANIMIMAGEINFO"; goto FILEFAILSAVE; }

	if (!fwrite(data.Image.data, data.Image.width * data.Image.height * 4, 1, file))
	{ errorsmg = "ANIMIMAGEDATA"; goto FILEFAILSAVE; }

	delete[] data.Frames;
	fclose(file);
	return;
FILEFAILSAVE:
	tinyfd_messageBox("EvilAnimator", TextFormat("Failed to save Animation (%s)", errorsmg), "ok", "error", 1);
	delete[] data.Frames;
	fclose(file);
}
*/

void ImGui_DrawInfoZone_AnimationEditor() {
	if (ImGui::Button("Create New Animator")) {
		const wchar_t* wpath = tinyfd_openFileDialogW(L"Choose Sprite", L"", 0, NULL, NULL, 0);
		if (wpath) {
			Texture2D texture = LoadTexture(WideToUTF8(wpath).c_str());
			if (texture.id != 0) {
				TempAnimationData* animdata = new TempAnimationData();
				animdata->Texture = texture;
				CurrentScene->CurrentData = animdata;
			}
			else {
				tinyfd_messageBox("EvilAnimator", "Failed to load Texture", "ok", "error", 1);
				UnloadTexture(texture);
			}
		}
	}
	ImGui::SameLine(0, 10);
	if (ImGui::Button("Load Animator")) {
		//DrawInfoZone_LoadAnimationFile();
	}
	if (CurrentScene->CurrentData) {
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Save Animator")) {
			//DrawInfoZone_SaveAnimationFile();
		}
		ImGui::NewLine();
		if (CurrentScene->AnimPlay.CurrentAnim) {
			ImGui::InputText("Animation Name", CurrentScene->AnimPlay.CurrentAnim->AnimName, MAX_ANIM_NLEN);
		}
		if (CurrentScene->CurrentData->Animations.size() > 0) {
			if (ImGui::SliderInt("Current Animation", &CurrentScene->AnimPlay.AnimIndex, 0, CurrentScene->CurrentData->Animations.size() - 1)) {
				if (CurrentScene->CurrentData->Animations.size() > 0) {
					std::list<TempAnimation>::iterator anim = CurrentScene->CurrentData->Animations.begin();
					std::advance(anim, CurrentScene->AnimPlay.AnimIndex);
					CurrentScene->AnimPlay.CurrentAnim = &(*anim);

					CurrentScene->AnimPlay.FrameIndex = 0;
					CurrentScene->AnimPlay.CurrentTime = 0;
					CurrentScene->AnimPlay.FramePoint = 0;
				}
			}
		}
		if (ImGui::Button("Add Animation")) {
			TempAnimation anim; 
			anim.AnimName = _strdup(TextFormat("Animation_%d", CurrentScene->CurrentData->Animations.size()));
		    CurrentScene->CurrentData->Animations.push_back(anim);

			if (CurrentScene->CurrentData->Animations.size() == 1) {
				CurrentScene->AnimPlay.AnimIndex = 0;
				CurrentScene->AnimPlay.CurrentAnim = &(*CurrentScene->CurrentData->Animations.begin());
			}
			else {
				CurrentScene->AnimPlay.AnimIndex++;
				std::list<TempAnimation>::iterator animil = CurrentScene->CurrentData->Animations.begin();
				std::advance(animil, CurrentScene->AnimPlay.AnimIndex);
				CurrentScene->AnimPlay.CurrentAnim = &(*animil);
			}

			AnimFrame frame = RL_Frame_CreateBlank(CurrentScene->CurrentData->Texture);
			frame.Pivot = Settings.Def_Pivot;
			CurrentScene->AnimPlay.CurrentAnim->Frames.push_back(frame);

			CurrentScene->AnimPlay.FrameIndex = 0;
			CurrentScene->AnimPlay.CurrentTime = 0;
			CurrentScene->AnimPlay.FramePoint = 0;
		}
	}


	if (CurrentScene->CurrentData && CurrentScene->CurrentData->Texture.id != 0) {
		ImGui::NewLine();
		rlImGuiImage(&CurrentScene->CurrentData->Texture);
	}
}

//FRAME
void ImGui_DrawInfoZone_FrameEditor() {

	if (!CurrentScene->CanDisplayFrames()) return;
	AnimFrame* frame = CurrentScene->AnimPlay.GetCurrentFrame();

	ImGui::PushItemWidth(100);

	ImGui::Text("Rect");

	ImGui::DragFloat("Rect X Pos", &frame->Rect.x);
	ImGui::SameLine(0, 10);
	ImGui::DragFloat("Rect Y Pos", &frame->Rect.y);

	ImGui::DragFloat("Rect Width", &frame->Rect.width);
	ImGui::SameLine(0, 10);
	ImGui::DragFloat("Rect Height", &frame->Rect.height);

	float pivot[2] = { frame->Pivot.x, frame->Pivot.y };
	if (ImGui::InputFloat2("Spr Pivot", pivot)) {
		frame->Pivot.x = pivot[0];
		frame->Pivot.y = pivot[1];
	}
	ImGui::SameLine(0, 10);
	float rotation = (float)frame->Rotation;
	if (ImGui::DragFloat("Spr Rotation", &rotation)) {
		frame->Rotation = rotation;
	}

	ImGui::NewLine();
	float color[4] = { (float)frame->color.r / 255.0f, (float)frame->color.g / 255.0f, (float)frame->color.b / 255.0f, (float)frame->color.a / 255.0f };
	if (ImGui::ColorPicker4("Spr Color", color)) {
		frame->color.r = (unsigned char)(color[0] * 255.0f);
		frame->color.g = (unsigned char)(color[1] * 255.0f);
		frame->color.b = (unsigned char)(color[2] * 255.0f);
		frame->color.a = (unsigned char)(color[3] * 255.0f);
	}
	ImGui::NewLine();

	ImGui::Text("Source");

	ImGui::InputFloat("Src X Pos", &frame->Source.x);
	ImGui::SameLine(0, 10);
	ImGui::InputFloat("Src Y Pos", &frame->Source.y);

	ImGui::InputFloat("Src Width", &frame->Source.width);
	ImGui::SameLine(0, 10);
	ImGui::InputFloat("Src Height", &frame->Source.height);

	if (CurrentScene->SprCells.size() > 0) {
		ImGui::NewLine();
		ImGui::SliderInt("Cells", &CurrentScene->CellIndex, 0, CurrentScene->SprCells.size() - 1);
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Set Source To Cell")) {
			auto cell_li = CurrentScene->SprCells.begin(); std::advance(cell_li, CurrentScene->CellIndex);
			frame->Source = *cell_li;
		}
	}

	ImGui::PopItemWidth();
}

//CELLS
void ImGui_DrawInfoZone_SpriteEditor_Viewer() {
	ImVec2 startPos = ImGui::GetCursorScreenPos();
	ImVec2 scaledif = { (float)CurrentScene->CurrentData->Texture.width, (float)CurrentScene->CurrentData->Texture.height };

	ImGui::GetWindowDrawList()->AddRect(
		{ startPos.x - 2,startPos.y - 2 },
		{ startPos.x + (scaledif.x + 2),startPos.y + (scaledif.y + 2) },
		IM_COL32(255, 255, 255, 255),
		0,
		0,
		2.5f
	);

	rlImGuiImageSize(&CurrentScene->CurrentData->Texture, scaledif.x, scaledif.y);

	if (!CurrentScene->SprCells.size()) return;
	auto cell_li = CurrentScene->SprCells.begin(); std::advance(cell_li, CurrentScene->CellIndex); SprRect* cell = &(*cell_li);

	ImGui::SetWindowFontScale(1.8f);
	int index = 0;
	for (SprRect& rect : CurrentScene->SprCells) {
		if (index != CurrentScene->CellIndex) {
			ImVec2 endpoint = { (startPos.x + rect.x) + rect.width,(startPos.y + rect.y) + rect.height };
			ImGui::GetWindowDrawList()->AddRect(
				{ startPos.x + rect.x,startPos.y + rect.y },
				{ endpoint },
				IM_COL32(0, 117, 44, 255),
				0, 0, 2.5f
			);
			ImGui::GetWindowDrawList()->AddText(
				{ (startPos.x + rect.x) + rect.width,(startPos.y + rect.y) - 14 },
				IM_COL32(0, 117, 44, 255),
				TextFormat("%d", index)
			);
		}
		index++;

	}
	ImGui::GetWindowDrawList()->AddRect(
		{ startPos.x + cell->x,startPos.y + cell->y },
		{ (startPos.x + cell->x) + cell->width,(startPos.y + cell->y) + cell->height },
		IM_COL32(0, 228, 48, 255),
		0, 0, 2.5f
	);
	ImGui::GetWindowDrawList()->AddText(
		{ (startPos.x + cell->x) + cell->width,(startPos.y + cell->y) - 14 },
		IM_COL32(0, 228, 48, 255),
		TextFormat("%d", CurrentScene->CellIndex)
	);
	ImGui::SetWindowFontScale(1);
}
void ImGui_DrawInfoZone_SpriteEditor() {
	if (!CurrentScene->CurrentData || CurrentScene->CurrentData->Texture.id == 0) return;

	if (ImGui::Button("Add Cell")) {
		CurrentScene->SprCells.push_back({ 0,0,(float)CurrentScene->CurrentData->Texture.width,(float)CurrentScene->CurrentData->Texture.height });

		if (CurrentScene->SprCells.size() == 1) {
			CurrentScene->CellIndex = 0;
		}
		else {
			CurrentScene->CellIndex++;
		}
	}

	if (CurrentScene->SprCells.size() > 0) {
		auto cell_li = CurrentScene->SprCells.begin(); std::advance(cell_li, CurrentScene->CellIndex); SprRect* cell = &(*cell_li);
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Reset Cell")) {
			*cell = { 0,0,(float)CurrentScene->CurrentData->Texture.width,(float)CurrentScene->CurrentData->Texture.height };
		}
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Delete Cell")) {
			CurrentScene->SprCells.erase(cell_li);
			CurrentScene->CellIndex--;
			if (!CurrentScene->SprCells.size()) return;
		}
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Copy Cell")) {
			CurrentScene->SprCells.push_back(*cell);
			CurrentScene->CellIndex++;
		}

		ImGui::SliderInt("Cells", &CurrentScene->CellIndex, 0, CurrentScene->SprCells.size() - 1);

		ImGui::DragFloat("Cell X Pos", &cell->x); ImGui::DragFloat("Cell Y Pos", &cell->y);
		ImGui::DragFloat("Cell Width", &cell->width); ImGui::DragFloat("Cell Height", &cell->height);
	}
	
	ImGui::NewLine();

	ImGui_DrawInfoZone_SpriteEditor_Viewer();
}

//SETTINGS
void ImGui_DrawInfoZone_Settings() {
	float def_piv[2] = { Settings.Def_Pivot.x, Settings.Def_Pivot.y };
	if (ImGui::InputFloat2("Default Pivot", def_piv)) {
		Settings.Def_Pivot = { def_piv[0], def_piv[1] };
	}

	ImGui::NewLine();

	if (ImGui::Button("Save Settings")) {
		
	}
}

//INFOZONE
void ImGui_DrawInfoZone() {

	float XLoc = ImGui::GetIO().DisplaySize.x / 2.5f;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - XLoc, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(XLoc, ImGui::GetIO().DisplaySize.y));

	ImGui::Begin("Animation Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	if (ImGui::Button("Animation Window")) {
		CurrentScene->ShoweCase = SINF_ANIM;
	}
	ImGui::SameLine(0, 10);
	if (ImGui::Button("Frame Window")) {
		CurrentScene->ShoweCase = SINF_FRAME;
	}
	ImGui::SameLine(0, 10);
	if (ImGui::Button("Sprite Window")) {
		CurrentScene->ShoweCase = SINF_SPRITE;
	}

	if (ImGui::Button("Settings Window")) {
		CurrentScene->ShoweCase = SINF_SETTINGS;
	}

	ImGui::NewLine();

	switch (CurrentScene->ShoweCase) {
	case SINF_ANIM: ImGui_DrawInfoZone_AnimationEditor(); break;
	case SINF_FRAME: ImGui_DrawInfoZone_FrameEditor(); break;
	case SINF_SPRITE: ImGui_DrawInfoZone_SpriteEditor(); break;
	case SINF_SETTINGS: ImGui_DrawInfoZone_Settings(); break;
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

//TIMELINE
void ImGui_DrawTimeLineVisualization() {

	ImVec2 startPos = ImGui::GetCursorScreenPos();

	static float timelineWidth = 550.0f;
	static float timelinemaxheight = 60;

	ImGui::GetWindowDrawList()->AddRectFilled({ startPos.x,startPos.y }, ImVec2(startPos.x + timelineWidth, startPos.y + 15), IM_COL32(80, 80, 80, 255));

	ImGui::GetWindowDrawList()->AddRectFilled({ startPos.x,startPos.y + 15 }, ImVec2(startPos.x + timelineWidth, startPos.y + 60), IM_COL32(255, 255, 255, 255));

	ImGui::GetWindowDrawList()->PushClipRect({ startPos.x,startPos.y }, { startPos.x + timelineWidth,startPos.y + timelinemaxheight }, true);

	static float framescale = 15;
	static int frameslotcount = (int)round(timelineWidth / framescale);
	float nextxpos = startPos.x;

	for (int i = 0; i < frameslotcount; i++) {
		ImGui::GetWindowDrawList()->AddRect({ startPos.x + (framescale * i),startPos.y + 15 }, { startPos.x + (framescale * (i + 1)),startPos.y + 60 }, IM_COL32(80, 80, 80, 255));
	}
	for (AnimFrame& frame : CurrentScene->AnimPlay.CurrentAnim->Frames) {
		float scaledframe = frame.Duration * framescale;
		ImGui::GetWindowDrawList()->AddRectFilled({ nextxpos,startPos.y + 15 }, { nextxpos + scaledframe, startPos.y + 60 }, IM_COL32(253, 249, 0, 255), 1);
		nextxpos = (nextxpos + scaledframe) + 1;
	}

}

void ImGui_DrawTimeLineZone() {

	float YLoc = ImGui::GetIO().DisplaySize.y / 2.5f;
	float XLoc = ImGui::GetIO().DisplaySize.x / 1.665f;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - YLoc), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(XLoc, YLoc + 1));

	ImGui::Begin("TimeLine", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	if (!CurrentScene->CanDisplayData()) {
		ImGui::End();
		ImGui::PopStyleColor();
		return;
	}

	if (CurrentScene->CanDisplayFrames()) {
		if (ImGui::SliderFloat("Current Time", &CurrentScene->AnimPlay.CurrentTime, 0, CurrentScene->AnimPlay.CurrentAnim->GetAnimLength())) {
			CurrentScene->AnimPlay.UpdateAnimation();
		}
		if (ImGui::SliderFloat("Frame Point", &CurrentScene->AnimPlay.FramePoint, 0, CurrentScene->AnimPlay.GetCurrentFrame()->Duration)) {
			CurrentScene->AnimPlay.CurrentTime 
				= CurrentScene->AnimPlay.CurrentAnim->GetTimePointByIndex(CurrentScene->AnimPlay.FrameIndex) + CurrentScene->AnimPlay.FramePoint;
		}
		if (ImGui::SliderInt("Current Frame", &CurrentScene->AnimPlay.FrameIndex, 0, CurrentScene->AnimPlay.CurrentAnim->Frames.size() - 1)) {
			CurrentScene->AnimPlay.CurrentTime = CurrentScene->AnimPlay.CurrentAnim->GetTimePointByIndex(CurrentScene->AnimPlay.FrameIndex);
			CurrentScene->AnimPlay.FramePoint = 0;
		}
		if (ImGui::Checkbox("Playing", &CurrentScene->AnimPlay.Playing)) {
			CurrentScene->AnimPlay.UpdateAnimation();
		}
		ImGui::SameLine(0, 10);
		if (ImGui::Checkbox("OnionSkin", &CurrentScene->AnimPlay.OnionSkin));
		ImGui::PushItemWidth(80);
		if (CurrentScene->AnimPlay.OnionSkin) {
			ImGui::SameLine(0, 10);
			ImGui::InputInt("OnionSkin Depth", &CurrentScene->AnimPlay.OnionSkin_Depth);
		}
		ImGui::PopItemWidth();
	}
	else {
		ImGui::Text("No Frames");
	}

	if (ImGui::Button("New Frame")) {
		AnimFrame newframe = RL_Frame_CreateBlank(CurrentScene->CurrentData->Texture);
		newframe.Pivot = Settings.Def_Pivot;
		#pragma endregion
		if (!CurrentScene->AnimPlay.CurrentAnim->Frames.size()) {
			CurrentScene->AnimPlay.CurrentAnim->Frames.push_back(newframe);
			CurrentScene->AnimPlay.FrameIndex = 0;
		}
		else {
			if (CurrentScene->AnimPlay.FrameIndex == CurrentScene->AnimPlay.CurrentAnim->Frames.size() - 1) {
				CurrentScene->AnimPlay.CurrentAnim->Frames.push_back(newframe);
			}
			else {
				auto frameli = CurrentScene->AnimPlay.CurrentAnim->GetFrameLIByIndex(CurrentScene->AnimPlay.FrameIndex);
				CurrentScene->AnimPlay.CurrentAnim->Frames.insert(frameli, newframe);
			}
			CurrentScene->AnimPlay.FrameIndex++;
		}
		CurrentScene->AnimPlay.UpdateAnimation();
		CurrentScene->AnimPlay.FramePoint = 0;
	}
	ImGui::SameLine(0, 10);
	if (CurrentScene->AnimPlay.CurrentAnim->Frames.size() > 0) {
		if (ImGui::Button("Copy Frame")) {
			AnimFrame copyframe = *CurrentScene->AnimPlay.GetCurrentFrame();
			if (CurrentScene->AnimPlay.FrameIndex == CurrentScene->AnimPlay.CurrentAnim->Frames.size() - 1) {
				CurrentScene->AnimPlay.CurrentAnim->Frames.push_back(copyframe);
			}
			else {
				auto frameli = CurrentScene->AnimPlay.CurrentAnim->GetFrameLIByIndex(CurrentScene->AnimPlay.FrameIndex);
				CurrentScene->AnimPlay.CurrentAnim->Frames.insert(frameli, copyframe);
			}
			CurrentScene->AnimPlay.FrameIndex++;
			CurrentScene->AnimPlay.UpdateAnimation();
			CurrentScene->AnimPlay.FramePoint = 0;
		}
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Delete Frame") && CurrentScene->AnimPlay.CurrentAnim->Frames.size() > 0) {
			CurrentScene->AnimPlay.CurrentAnim->Frames.erase(CurrentScene->AnimPlay.CurrentAnim->GetFrameLIByIndex(CurrentScene->AnimPlay.FrameIndex));
			CurrentScene->AnimPlay.FrameIndex = std::max(0, CurrentScene->AnimPlay.FrameIndex - 1);
			CurrentScene->AnimPlay.UpdateAnimation();
			CurrentScene->AnimPlay.FramePoint = 0;
		}

		if (CurrentScene->AnimPlay.FramePoint > 0 && CurrentScene->AnimPlay.FramePoint < CurrentScene->AnimPlay.CurrentAnim->Frames.size() - 1) {
			if (ImGui::Button("Move Frame Left")) {
				CurrentScene->AnimPlay.CurrentAnim->ShiftFrame(CurrentScene->AnimPlay.FrameIndex, -1);
				CurrentScene->AnimPlay.FrameIndex--;
				CurrentScene->AnimPlay.UpdateAnimation();
			}
			ImGui::SameLine(0, 10);
			if (ImGui::Button("Move Frame Right")) {
				CurrentScene->AnimPlay.CurrentAnim->ShiftFrame(CurrentScene->AnimPlay.FrameIndex, 1);
				CurrentScene->AnimPlay.FrameIndex--;
				CurrentScene->AnimPlay.UpdateAnimation();
			}
		}
		else {
			if (CurrentScene->AnimPlay.FramePoint > 0) {
				if (ImGui::Button("Move Frame Left")) {
					CurrentScene->AnimPlay.CurrentAnim->ShiftFrame(CurrentScene->AnimPlay.FrameIndex, -1);
					CurrentScene->AnimPlay.FrameIndex--;
					CurrentScene->AnimPlay.UpdateAnimation();
				}
			}
			if (CurrentScene->AnimPlay.FramePoint < CurrentScene->AnimPlay.CurrentAnim->Frames.size() - 1) {
				if (ImGui::Button("Move Frame Right")) {
					CurrentScene->AnimPlay.CurrentAnim->ShiftFrame(CurrentScene->AnimPlay.FrameIndex, 1);
					CurrentScene->AnimPlay.FrameIndex--;
					CurrentScene->AnimPlay.UpdateAnimation();
				}
			}
		}
	}

	AnimFrame* frame = CurrentScene->AnimPlay.GetCurrentFrame();
	if (frame) {
		ImGui::Text("Frame");
		ImGui::SameLine(0, 10);
		ImGui::PushItemWidth(100);
		if (ImGui::InputFloat("Frame Duration", &frame->Duration)) {
			float ndur = std::fmaxf(0, frame->Duration);
			CurrentScene->AnimPlay.CurrentTime + ndur - frame->Duration;
			frame->Duration = ndur;
			CurrentScene->AnimPlay.CurrentTime = CurrentScene->AnimPlay.CurrentAnim->GetTimePointByIndex(CurrentScene->AnimPlay.FrameIndex);
			CurrentScene->AnimPlay.FramePoint = 0;
		}
		ImGui::PopItemWidth();

		ImGui::SameLine(0, 10);

		ImGui::Checkbox("Interpolate", (bool*)&frame->Interpolate);
	}

	ImGui::NewLine();

	//ImGui_DrawTimeLineVisualization();

	ImGui::End();
	ImGui::PopStyleColor();
}

int main()
{
	InitWindow(1024, 600, "EvilAnimator");
	rlImGuiSetup(true);

	SetExitKey(KEY_F12);

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GRAY);

		DrawSpriteDisplay();

		rlImGuiBegin();

		ImGui_DrawInfoZone();
		ImGui_DrawTimeLineZone();

		rlImGuiEnd();

		EndDrawing();
	}
	CloseWindow();
	return 1;
}

