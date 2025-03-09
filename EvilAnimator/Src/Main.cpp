
#include <raylib.h>

#include "Utils/Rl_ImGui/imgui.h"
#include "Utils/Rl_ImGui/rlImGui.h"
#include "Utils/TinyFd/tinyfiledialogs.h"

#include "Animation/AnimationScene.h"

#include <string>
#include <codecvt>

AnimationScene* CurrentScene;

std::string WideToUTF8(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

void DrawSpriteDisplay() {
	if (CurrentScene->AnimPlay.Playing) {
		if (!CurrentScene->CurrentData || CurrentScene->CurrentData->Texture.id == 0 || !CurrentScene->CurrentData->Frames.size()) {
			CurrentScene->AnimPlay.Playing = false;
			return;
		}
		CurrentScene->TickFrames();
	}

	float MiddlePoint_X = GetRenderWidth() / 4;
	float MiddlePoint_Y = GetRenderHeight() / 4;

	DrawLineEx({ MiddlePoint_X,0 }, { MiddlePoint_X,(float)GetRenderHeight() }, 2.5f, DARKGRAY);
	DrawLineEx({ 0,MiddlePoint_Y }, { (float)GetRenderWidth(), MiddlePoint_Y }, 2.5f, DARKGRAY);

	DrawRectangleLinesEx({ MiddlePoint_X, MiddlePoint_Y, 32, 32 }, 2, DARKGRAY);

	CurrentScene->DrawAnimationFrame({ MiddlePoint_X,MiddlePoint_Y });
	CurrentScene->UpdateSpriteHandling(GetMousePosition(), { MiddlePoint_X,MiddlePoint_Y });
}

void ImGui_DrawInfoZone_AnimationEditor() {

	AnimFrame* frame = CurrentScene->AnimPlay.CurrentFrame;
	if (frame) {
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

		ImGui::PopItemWidth();

		ImGui::NewLine();
	}

	if (CurrentScene->CurrentData && CurrentScene->CurrentData->Texture.id != 0) {
		rlImGuiImage(&CurrentScene->CurrentData->Texture);
		ImGui::NewLine();
	}

	if (ImGui::Button("Create New Animation")) {
		const wchar_t* wpath = tinyfd_openFileDialogW(L"Open File", L"", 0, NULL, NULL, 0);
		if (wpath) {
			CurrentScene->CurrentData = new TempAnimationData();

			Texture2D texture = LoadTexture(WideToUTF8(wpath).c_str());
			if (texture.id != 0) {
				if (CurrentScene->CurrentData->Texture.id != 0) UnloadTexture(CurrentScene->CurrentData->Texture);
				CurrentScene->CurrentData->Texture = texture;

				AnimFrame frame;
				frame.Source = { 0,0,(float)CurrentScene->CurrentData->Texture.width,(float)CurrentScene->CurrentData->Texture.height };
				frame.Rect = { 0,0,1,1 };
				frame.Rotation = 0;
				frame.Pivot = { 0.5f,0.5f };
				frame.Duration = 1;
				frame.color = WHITE;
				CurrentScene->CurrentData->Frames.clear();
				CurrentScene->CurrentData->Frames.push_back(frame);
				CurrentScene->UpdateSelectedFrame();
				CurrentScene->SprCells.clear();
				CurrentScene->SprCells.push_back(frame.Source);
			}
		}
	}
}

void ImGui_DrawInfoZone_SpriteEditor() {
	if (!CurrentScene || !CurrentScene->CurrentData) return;

	if (ImGui::Button("Add Cell")) {
		CurrentScene->SprCells.push_back({ 0,0,(float)CurrentScene->CurrentData->Texture.width,(float)CurrentScene->CurrentData->Texture.height });
		CurrentScene->CellIndex++;
	}
	if (!CurrentScene->SprCells.size()) return;
	ImGui::SameLine(0, 10);
	auto cell_li = CurrentScene->SprCells.begin(); std::advance(cell_li, CurrentScene->CellIndex); Rectangle* cell = &(*cell_li);
	if (ImGui::Button("Copy Cell")) {
		CurrentScene->SprCells.push_back(*cell);
		CurrentScene->CellIndex++;
	}

	ImGui::SliderInt("Cells", &CurrentScene->CellIndex, 0, CurrentScene->SprCells.size()-1);

	ImGui::DragFloat("Cell X Pos", &cell->x); ImGui::DragFloat("Cell Y Pos", &cell->y);
	ImGui::DragFloat("Cell Width", &cell->width); ImGui::DragFloat("Cell Height", &cell->height);

	ImVec2 startPos = ImGui::GetCursorScreenPos();
	ImVec2 scaledif = { abs((float)CurrentScene->CurrentData->Texture.width - 192), abs((float)CurrentScene->CurrentData->Texture.height - 192) };

	ImGui::GetWindowDrawList()->AddRect({ startPos.x - 2,startPos.y - 2 }, { startPos.x + 194,startPos.y + 194 }, IM_COL32(255, 255, 255, 255), 0, 0, 2.5f);
	rlImGuiImageSize(&CurrentScene->CurrentData->Texture, 192, 192);

	ImGui::SetWindowFontScale(1.8f);
	int index = 0;
	for (Rectangle& rect : CurrentScene->SprCells) {
		if (index != CurrentScene->CellIndex) {
			ImVec2 endpoint = { (startPos.x + rect.x) + (rect.width + scaledif.x),(startPos.y + rect.y) + (rect.height + scaledif.y) };
			ImGui::GetWindowDrawList()->AddRect(
				{ startPos.x + rect.x,startPos.y + rect.y },
				{ endpoint },
				IM_COL32(0, 117, 44, 255),
				0, 0, 2.5f
			);
			ImGui::GetWindowDrawList()->AddText(
				{ (startPos.x + rect.x) + (rect.width + scaledif.x),(startPos.y + rect.y) - 14 },
				IM_COL32(0, 117, 44, 255),
				TextFormat("%d",index)
			);
		}
		index++;

	}
	ImGui::GetWindowDrawList()->AddRect(
		{ startPos.x + cell->x,startPos.y + cell->y },
		{ (startPos.x + cell->x) + (cell->width + scaledif.x),(startPos.y + cell->y) + (cell->height + scaledif.y) },
		IM_COL32(0, 228, 48, 255),
		0, 0, 2.5f
	);
	ImGui::GetWindowDrawList()->AddText(
		{ (startPos.x + cell->x) + (cell->width + scaledif.x),(startPos.y+cell->y)-14 }, 
		IM_COL32(0, 228, 48, 255),
		TextFormat("%d",CurrentScene->CellIndex)
	);
	ImGui::SetWindowFontScale(1);
}

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
	if (ImGui::Button("Sprite Window")) {
		CurrentScene->ShoweCase = SINF_SPRITE;
	}

	ImGui::NewLine();

	switch (CurrentScene->ShoweCase) {
	case SINF_ANIM: ImGui_DrawInfoZone_AnimationEditor(); break;
	case SINF_SPRITE: ImGui_DrawInfoZone_SpriteEditor(); break;
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

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
	for (AnimFrame& frame : CurrentScene->CurrentData->Frames) {
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

	if (!CurrentScene->CurrentData) {
		ImGui::End();
		ImGui::PopStyleColor();
		return;
	}
	if (CurrentScene->CurrentData->Frames.size() > 0) {
		if (ImGui::SliderFloat("Current Time", &CurrentScene->AnimPlay.CurrentTime, 0, CurrentScene->GetAnimationTime())) {
			CurrentScene->UpdateFramePoint();
		}
		if (ImGui::SliderFloat("Frame Point", &CurrentScene->AnimPlay.FramePoint, 0, CurrentScene->AnimPlay.CurrentFrame->Duration)) {
		}
		if (ImGui::SliderInt("Current Frame", &CurrentScene->FrameIndex, 0, CurrentScene->CurrentData->Frames.size() - 1)) {
			CurrentScene->UpdateSelectedFrame();
			CurrentScene->AnimPlay.CurrentTime = CurrentScene->GetCurrentFramePoint();
			CurrentScene->AnimPlay.FramePoint = 0;
		}
		if (ImGui::Checkbox("Playing", &CurrentScene->AnimPlay.Playing)) {
			CurrentScene->UpdateFramePoint();
		}
		/*
		else if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
			CurrentScene->AnimPlay.Playing = false;
		}
		*/
	}
	else {
		ImGui::Text("No Frames");
	}

	if (ImGui::Button("New Frame")) {
		AnimFrame newframe;
		#pragma region FrameConstruct
		newframe.Source = { 0,0,(float)CurrentScene->CurrentData->Texture.width,(float)CurrentScene->CurrentData->Texture.height };
		newframe.Rect = { 0,0,1,1 };
		newframe.Rotation = 0;
		newframe.Pivot = { 0.5f, 0.5f };
		newframe.Duration = 1;
		newframe.color = WHITE;
		#pragma endregion
		if (CurrentScene->FrameIndex == CurrentScene->CurrentData->Frames.size() - 1 || !CurrentScene->CurrentData->Frames.size()) {
			CurrentScene->CurrentData->Frames.push_back(newframe);
		}
		else {
			auto frameli = CurrentScene->GetCurrentFrame_LI();
			CurrentScene->CurrentData->Frames.insert(CurrentScene->GetCurrentFrame_LI(), newframe);
		}
		CurrentScene->FrameIndex++;
		CurrentScene->UpdateSelectedFrame();
		CurrentScene->UpdateFramePoint();
	}
	ImGui::SameLine(0, 10);
	if (CurrentScene->CurrentData->Frames.size() > 0) {
		if (ImGui::Button("Copy Frame")) {
			AnimFrame copyframe = *CurrentScene->GetCurrentFrame();
			if (CurrentScene->FrameIndex == CurrentScene->CurrentData->Frames.size() - 1) {
				CurrentScene->CurrentData->Frames.push_back(copyframe);
			}
			else {
				auto frameli = CurrentScene->GetCurrentFrame_LI();
				CurrentScene->CurrentData->Frames.insert(CurrentScene->GetCurrentFrame_LI(), copyframe);
			}
			CurrentScene->FrameIndex++;
			CurrentScene->UpdateSelectedFrame();
		}
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Delete Frame") && CurrentScene->CurrentData->Frames.size() > 0) {
			auto frameli = CurrentScene->GetCurrentFrame_LI();
			CurrentScene->CurrentData->Frames.erase(frameli);
			CurrentScene->FrameIndex = std::max(0, CurrentScene->FrameIndex - 1);
			CurrentScene->UpdateSelectedFrame();
			CurrentScene->UpdateSelectedFrame();
		}

		if (CurrentScene->FrameIndex > 0 && CurrentScene->FrameIndex < CurrentScene->CurrentData->Frames.size() - 1) {
			if (ImGui::Button("Move Frame Left")) {
				CurrentScene->MoveCurrentFrame(-1);
				CurrentScene->FrameIndex--;
				CurrentScene->UpdateSelectedFrame();
				CurrentScene->UpdateFramePoint();
			}
			ImGui::SameLine(0, 10);
			if (ImGui::Button("Move Frame Right")) {
				CurrentScene->MoveCurrentFrame(1);
				CurrentScene->FrameIndex++;
				CurrentScene->UpdateSelectedFrame();
				CurrentScene->UpdateFramePoint();
			}
		}
		else {
			if (CurrentScene->FrameIndex > 0) {
				if (ImGui::Button("Move Frame Left")) {
					CurrentScene->MoveCurrentFrame(-1);
					CurrentScene->FrameIndex--;
					CurrentScene->UpdateSelectedFrame();
					CurrentScene->UpdateFramePoint();
				}
			}
			if (CurrentScene->FrameIndex < CurrentScene->CurrentData->Frames.size() - 1) {
				if (ImGui::Button("Move Frame Right")) {
					CurrentScene->MoveCurrentFrame(1);
					CurrentScene->FrameIndex++;
					CurrentScene->UpdateSelectedFrame();
					CurrentScene->UpdateFramePoint();
				}
			}
		}
	}

	AnimFrame* frame = CurrentScene->GetCurrentFrame();
	if (frame) {
		ImGui::Text("Frame");
		ImGui::SameLine(0, 10);
		ImGui::PushItemWidth(100);
		if (ImGui::InputFloat("Frame Duration", &frame->Duration)) {
			float ndur = std::fmaxf(0, frame->Duration);
			CurrentScene->AnimPlay.CurrentTime + ndur - frame->Duration;
			frame->Duration = ndur;
			CurrentScene->UpdateFramePoint();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine(0, 10);
		if (ImGui::Checkbox("Interpolate",&frame->Interpolate)) {

		}
	}

	ImGui::NewLine();

	//ImGui_DrawTimeLineVisualization();

	ImGui::End();
	ImGui::PopStyleColor();
}

void DrawScene() {
	DrawText(TextFormat("View Scale: %f", 2.0f), 10, 10, 18, BLACK);
}

int main()
{
	InitWindow(1024, 768, "EvilAnimator");
	rlImGuiSetup(true);

	SetExitKey(KEY_F12);

	Texture2D spr = LoadTexture("Resources/BugIcon.png");
	Sprite* sprite = new Sprite(&spr);

	CurrentScene = new AnimationScene();

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

