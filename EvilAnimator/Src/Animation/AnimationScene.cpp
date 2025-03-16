
#include "AnimationScene.h"
#include <algorithm>
#include <math.h>
#include <cmath>

Rectangle CalcDrawRect(AnimFrame frame, Vector2 pos) {
	float scaledWidth = frame.Source.width * frame.Rect.width;
	float scaledHeight = frame.Source.height * frame.Rect.height;

	Rectangle destRect = {
		(pos.x + frame.Rect.x),
		(pos.y + frame.Rect.y),
		scaledWidth,
		scaledHeight
	};

	destRect.x -= frame.Pivot.x * destRect.width;
	destRect.y -= frame.Pivot.y * destRect.height;

	return destRect;
}

//

TempAnimation* TempAnimationData::GetAnimByIndex(int index) {
	if (index < 0 || index >= Animations.size() - 1) return nullptr;
	std::list<TempAnimation>::iterator anim = Animations.begin(); std::advance(anim, index);
	return &(*anim);
}

//

float TempAnimation::GetAnimLength() {
	float combinetime = 0;
	for (AnimFrame& frame : Frames) 
		combinetime += frame.Duration;
	return combinetime;
}
float TempAnimation::GetTimePointByIndex(int index) {
	float combinetime = 0; int i = 0;
	for (AnimFrame& frame : Frames) {
		if (i == index) return combinetime;
		combinetime += frame.Duration;
		i++;
	}
	return combinetime;
}

void TempAnimation::ShiftFrame(int index, int dist) {
	if (index < 0 || index >= Frames.size()) return;

	std::list<AnimFrame>::iterator sw_frame = Frames.begin(); 
	std::advance(sw_frame, index);

	if (dist == 1) {
		auto next_frame = std::next(sw_frame);
		if (next_frame != Frames.end()) {
			std::iter_swap(sw_frame, next_frame);
		}
	}
	else if (dist == -1) {
		if (sw_frame != Frames.begin()) {
			auto prev_frame = std::prev(sw_frame);
			std::iter_swap(sw_frame, prev_frame);
		}
	}
}

AnimFrame* TempAnimation::GetFrameByIndex(int index) {
	if (index < 0 || index >= Frames.size()) return nullptr;
	std::list<AnimFrame>::iterator frame = Frames.begin(); std::advance(frame, index);
	return &(*frame);
}
std::list<AnimFrame>::iterator TempAnimation::GetFrameLIByIndex(int index) {
	if (index < 0 || index >= Frames.size()) return {};
	std::list<AnimFrame>::iterator frame = Frames.begin(); std::advance(frame, index);
	return frame;
}

int TempAnimation::GetFrameIndexAtTimePoint(float time) {
	float combinetime = 0; int i = 0;
	for (AnimFrame& frame : Frames) {
		combinetime += frame.Duration;
		if (time <= combinetime) return i;
		i++;
	}
	return combinetime;
}

//

void SpriteAnimPlay::TickAnimation() {
	if (!Playing) return;
	CurrentTime += GetFrameTime();
	FramePoint += GetFrameTime();
	if (CurrentAnim->GetFrameByIndex(FrameIndex)->Duration <= FramePoint) {
		if (FrameIndex >= CurrentAnim->Frames.size() - 1) {
			CurrentTime = CurrentTime - CurrentAnim->GetAnimLength();
			FramePoint = GetFramePoint();
			FrameIndex = 0;
			return;
		}
		FrameIndex++;
		FramePoint = GetFramePoint();
	}
}
void SpriteAnimPlay::UpdateAnimation() {
	FramePoint = GetFramePoint();
	FrameIndex = CurrentAnim->GetFrameIndexAtTimePoint(CurrentTime);
}

float SpriteAnimPlay::GetFramePoint() {
	float timeoffset = 0;
	auto frame_li = CurrentAnim->Frames.begin();
	for (int i = 0; i < CurrentAnim->Frames.size(); i++, ++frame_li) {
		if (CurrentTime < timeoffset + frame_li->Duration) {
			return CurrentTime - timeoffset;
		}
		timeoffset += frame_li->Duration;
	}
	return timeoffset;
}
AnimFrame* SpriteAnimPlay::GetCurrentFrame() {
	return CurrentAnim->GetFrameByIndex(FrameIndex);
}

void SpriteAnimPlay::DrawOnionFrames(Texture2D tex, Vector2 pos) {
	if (!CurrentAnim || !CurrentAnim->Frames.size()) return;

	int startindex = std::max(0, FrameIndex - OnionSkin_Depth); int depth = (OnionSkin_Depth * 2) + 1;
	auto frame_li = CurrentAnim->Frames.begin(); std::advance(frame_li, startindex);

	for (int i = 0; i < depth; i++, startindex++, ++frame_li) {
		if (startindex > CurrentAnim->Frames.size() - 1) break;
		if (i == FrameIndex) continue;

		AnimFrame frame = *frame_li;

		Rectangle destRect = {
		pos.x + frame.Rect.x,
		pos.y + frame.Rect.y,
		frame.Source.width * frame.Rect.width,
		frame.Source.height * frame.Rect.height
		};

		Vector2 pivot = {
			frame.Pivot.x * destRect.width,
			frame.Pivot.y * destRect.height
		};

		DrawTexturePro(tex, *(Rectangle*)&frame.Source, destRect, pivot, frame.Rotation, Color{ 0, 228, 48, 125 });
		Rectangle rect = CalcDrawRect(frame, pos);
		DrawText(TextFormat("%d", startindex - FrameIndex), rect.x, rect.y, 14, Color{ 0, 228, 48, 125 });
	}
}
void SpriteAnimPlay::DrawAnimationFrame(Texture2D tex, Vector2 pos) {
	if (!CurrentAnim || !CurrentAnim->Frames.size()) return;

	AnimFrame cframe = *CurrentAnim->GetFrameByIndex(FrameIndex);
	if (cframe.Interpolate && FrameIndex + 1 < CurrentAnim->Frames.size())
		cframe = Frame_Interpolate(cframe, *CurrentAnim->GetFrameByIndex(FrameIndex + 1), FramePoint / cframe.Duration);

	Rectangle destRect = {
		pos.x + cframe.Rect.x,
		pos.y + cframe.Rect.y,
		cframe.Source.width * cframe.Rect.width,
		cframe.Source.height * cframe.Rect.height
	};

	Vector2 pivot = {
		cframe.Pivot.x * destRect.width,
		cframe.Pivot.y * destRect.height
	};

	DrawTexturePro(tex, *(Rectangle*)&cframe.Source, destRect, pivot, cframe.Rotation, *(Color*)&cframe.color);
}

//

void AnimationScene::UnloadScene() {
	if (CurrentData) {
		if (CurrentData->Texture.id == 0)
			UnloadTexture(CurrentData->Texture);

		//CurrentData->Frames.clear();

		delete CurrentData;
	}
	SprCells.clear();
	
	AnimPlay = SpriteAnimPlay();
	SprInterface = SpriteInterface();

	//FrameIndex = 0;
	CellIndex = 0;
}
void AnimationScene::LoadScene(AnimSaveData data) {
	if (CurrentData) UnloadScene();

	CurrentData = new TempAnimationData();

	Image img;
	img.width = data.Image.width; img.height = data.Image.height;
	img.format = data.Image.format; img.mipmaps = data.Image.mipmaps;
	img.data = data.Image.data;

	CurrentData->Texture = LoadTextureFromImage(img);

	for (int i = 0; i < data.FrameCount; i++) {
		//CurrentData->Frames.push_back(data.Frames[i]);
	}

	//AnimPlay.CurrentFrame = GetCurrentFrame();
}

bool AnimationScene::CanDisplayAnim() {
	return !(!CurrentData || !CurrentData->Animations.size() || CurrentData->Texture.id == 0 || !AnimPlay.CurrentAnim || !AnimPlay.CurrentAnim->Frames.size());
}
bool AnimationScene::CanDisplayFrames() {
	return !(!CurrentData || !CurrentData->Animations.size() || !AnimPlay.CurrentAnim || !AnimPlay.CurrentAnim->Frames.size());
}
bool AnimationScene::CanDisplayData() {
	return !(!CurrentData || !CurrentData->Animations.size() || !AnimPlay.CurrentAnim);
}

void AnimationScene::ConvertCurrentSceneToAnimData(AnimSaveData* data) {

	//data->FrameCount = CurrentData->Frames.size();
	//data->Frames = new AnimFrame[CurrentData->Frames.size()];

	//std::copy(CurrentData->Frames.begin(), CurrentData->Frames.end(), data->Frames);
	
	Image img = LoadImageFromTexture(CurrentData->Texture);
	size_t datasize = img.width * img.height * 4; 
	data->Image.data = (void*)malloc(datasize);

	data->Image.width = img.width; data->Image.height = img.height;
	data->Image.format = img.format; data->Image.mipmaps = img.mipmaps;
	memcpy_s(data->Image.data, datasize, img.data, datasize);

	UnloadImage(img);
}
void AnimationScene::UpdateSpriteHandling(Vector2 mpos, Vector2 dpos) {
	//if (CanDisplayAnim()) return;

	AnimFrame* cframe = AnimPlay.CurrentAnim->GetFrameByIndex(AnimPlay.FrameIndex);
	AnimFrame rframe = *cframe;

	if (rframe.Interpolate && AnimPlay.FrameIndex < AnimPlay.CurrentAnim->Frames.size() - 1)
		rframe = Frame_Interpolate(rframe, *AnimPlay.CurrentAnim->GetFrameByIndex(AnimPlay.FrameIndex + 1), AnimPlay.FramePoint / rframe.Duration);

	#pragma region Define

	int LocX = dpos.x + rframe.Rect.x + rframe.Rect.width / 2 - rframe.Pivot.x;
	int LocY = dpos.y + rframe.Rect.y + rframe.Rect.height / 2 - rframe.Pivot.y;

	int handleSize = 8;
	int arrowLength = 32;
	int arrowThickness = 8;

	#pragma endregion

	bool overBase = CheckCollisionPointRec(mpos, { (float)(LocX - handleSize / 2), (float)(LocY - handleSize / 2), (float)handleSize, (float)handleSize });
	bool overXArrow = CheckCollisionPointLine(mpos, { (float)LocX, (float)LocY }, { (float)(LocX + arrowLength), (float)LocY }, arrowThickness);
	bool overYArrow = CheckCollisionPointLine(mpos, { (float)LocX, (float)LocY }, { (float)LocX, (float)(LocY - arrowLength) }, arrowThickness);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		SprInterface.LastMousePos = mpos;
		SprInterface.LastRectPos = { cframe->Rect.x, cframe->Rect.y };

		if (!(SprInterface.Selected && (overBase || overXArrow || overYArrow))) {
			SprInterface.Selected = CheckCollisionPointRec(mpos, CalcDrawRect(rframe, dpos));
		}
	}

	if (!SprInterface.Selected) return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (overBase) {
			SprInterface.DragX = true;
			SprInterface.DragY = true;
		}
		else if (overXArrow) SprInterface.DragX = true;
		else if (overYArrow) SprInterface.DragY = true;
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		SprInterface.DragX = false;
		SprInterface.DragY = false;
	}

	if (SprInterface.DragX || SprInterface.DragY) {
		float offsetX = mpos.x - SprInterface.LastMousePos.x;
		float offsetY = mpos.y - SprInterface.LastMousePos.y;

		if (SprInterface.DragX) {
			cframe->Rect.x = SprInterface.LastRectPos.x + offsetX;
		}
		if (SprInterface.DragY) {
			cframe->Rect.y = SprInterface.LastRectPos.y + offsetY;
		}
	}

    #pragma region DrawTranslationTool
	DrawLineEx(
		{ (float)LocX, (float)LocY },
		{ (float)LocX, (float)(LocY - arrowLength) },
		4, overYArrow ? YELLOW : GREEN
	);
	DrawTriangle(
		{ (float)LocX, (float)(LocY - arrowLength - 6) },
		{ (float)(LocX - 6), (float)(LocY - arrowLength) },
		{ (float)(LocX + 6), (float)(LocY - arrowLength) },
		overYArrow ? YELLOW : GREEN
	);
	DrawLineEx(
		{ (float)LocX, (float)LocY },
		{ (float)(LocX + arrowLength), (float)LocY },
		4, overXArrow ? YELLOW : RED
	);
	DrawTriangle(
		{ (float)(LocX + arrowLength + 6), (float)LocY },
		{ (float)(LocX + arrowLength), (float)(LocY - 6) },
		{ (float)(LocX + arrowLength), (float)(LocY + 6) },
		overXArrow ? YELLOW : RED
	);

	DrawRectangle(LocX - handleSize / 2, LocY - handleSize / 2, handleSize, handleSize, overBase ? GRAY : BLACK);
    #pragma endregion
}