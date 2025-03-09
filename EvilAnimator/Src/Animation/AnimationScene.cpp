
#include "AnimationScene.h"
#include <algorithm>
#include <math.h>
#include <cmath>

template <typename T>
constexpr T lerp(T a, T b, T t) {
	return a + t * (b - a);
}

void AnimationScene::TickFrames() {
	float newtime = AnimPlay.CurrentTime += GetFrameTime();
	AnimPlay.FramePoint += GetFrameTime();
	if (AnimPlay.CurrentFrame->Duration <= AnimPlay.FramePoint) {
		if (FrameIndex + 1 >= CurrentData->Frames.size()) {
			FrameIndex = 0;
			AnimPlay.FramePoint = 0;
			AnimPlay.CurrentTime = 0;
			AnimPlay.CurrentFrame = GetCurrentFrame();
			return;
		}
		else {
			FrameIndex++;
		}
		AnimPlay.CurrentFrame = GetCurrentFrame();
		UpdateFramePoint();
	}
	AnimPlay.CurrentTime = newtime;
}
void AnimationScene::UpdateFramePoint() {
	float timeoffset = 0;
	for (AnimFrame& frame : CurrentData->Frames) {
		if (AnimPlay.CurrentTime < timeoffset + frame.Duration) {
			AnimPlay.CurrentFrame = &frame;
			AnimPlay.FramePoint = AnimPlay.CurrentTime - timeoffset;
			return;
		}
		timeoffset += frame.Duration;
	}
}
void AnimationScene::UpdateSelectedFrame() {
	AnimPlay.CurrentFrame = GetCurrentFrame();
}

float AnimationScene::GetAnimationTime() {
	float frametime = 0;
	for (AnimFrame& frame : CurrentData->Frames) {
		frametime += frame.Duration;
	}
	return frametime;
}
float AnimationScene::GetCurrentFramePoint() {
	float frametime = 0; int index = 0;
	for (AnimFrame& frame : CurrentData->Frames) {
		if (FrameIndex == index) return frametime;
		frametime += frame.Duration;
		index++;
	}
	return 0;
}

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
void AnimationScene::UpdateSpriteHandling(Vector2 mpos, Vector2 dpos) {
	if (!CurrentData || CurrentData->Texture.id == 0 || !CurrentData->Frames.size()) return;

	AnimFrame* frame = AnimPlay.CurrentFrame;

	#pragma region Define

	int LocX = dpos.x + frame->Rect.x + frame->Rect.width / 2 - frame->Pivot.x;
	int LocY = dpos.y + frame->Rect.y + frame->Rect.height / 2 - frame->Pivot.y;

	int handleSize = 8;
	int arrowLength = 32;
	int arrowThickness = 8;

	#pragma endregion

	bool overBase = CheckCollisionPointRec(mpos, { (float)(LocX - handleSize / 2), (float)(LocY - handleSize / 2), (float)handleSize, (float)handleSize });
	bool overXArrow = CheckCollisionPointLine(mpos, { (float)LocX, (float)LocY }, { (float)(LocX + arrowLength), (float)LocY }, arrowThickness);
	bool overYArrow = CheckCollisionPointLine(mpos, { (float)LocX, (float)LocY }, { (float)LocX, (float)(LocY - arrowLength) }, arrowThickness);
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		SprInterface.LastMousePos = mpos;
		SprInterface.LastRectPos = { frame->Rect.x, frame->Rect.y };

		if (!(SprInterface.Selected && (overBase || overXArrow || overYArrow))) {
			SprInterface.Selected = CheckCollisionPointRec(mpos, CalcDrawRect(AnimPlay.CurrentFrame->Interpolate ? GetCurrentFrameInterpolateData() : *AnimPlay.CurrentFrame, dpos));
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
			frame->Rect.x = SprInterface.LastRectPos.x + offsetX;
		}
		if (SprInterface.DragY) {
			frame->Rect.y = SprInterface.LastRectPos.y + offsetY;
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

AnimFrame* AnimationScene::GetCurrentFrame() {
	if (!CurrentData || CurrentData->Texture.id == 0 || !CurrentData->Frames.size()) return nullptr;
	std::list<AnimFrame>::iterator frame = CurrentData->Frames.begin();
	std::advance(frame, std::min(FrameIndex, (int)CurrentData->Frames.size() - 1));
	return &(*frame);
}
AnimFrame* AnimationScene::GetNextFrame() {
	if (!CurrentData || CurrentData->Texture.id == 0 || !CurrentData->Frames.size()) return nullptr;
	if (FrameIndex + 1 > CurrentData->Frames.size() - 1) return nullptr;
	std::list<AnimFrame>::iterator frame = CurrentData->Frames.begin();
	std::advance(frame, std::min(FrameIndex+1, (int)CurrentData->Frames.size() - 1));
	return &(*frame);
}
std::list<AnimFrame>::iterator AnimationScene::GetCurrentFrame_LI() {
	if (!CurrentData || CurrentData->Texture.id == 0 || !CurrentData->Frames.size()) return {};
	std::list<AnimFrame>::iterator frame = CurrentData->Frames.begin();
	std::advance(frame, std::min(FrameIndex, (int)CurrentData->Frames.size() - 1));
	return frame;
}

void AnimationScene::MoveCurrentFrame(int dist) {
	auto sw_frame = GetCurrentFrame_LI();

	if (dist == 1) {
		auto next_frame = std::next(sw_frame);
		if (next_frame != CurrentData->Frames.end()) {
			std::iter_swap(sw_frame, next_frame);
		}
	}
	else if (dist == -1) {
		if (sw_frame != CurrentData->Frames.begin()) {
			auto prev_frame = std::prev(sw_frame);
			std::iter_swap(sw_frame, prev_frame);
		}
	}
}

AnimFrame AnimationScene::GetCurrentFrameInterpolateData() {
	if (FrameIndex == CurrentData->Frames.size() - 1 || CurrentData->Frames.size() == 1) return *AnimPlay.CurrentFrame;

	auto cframe = AnimPlay.CurrentFrame; auto nframe = GetNextFrame();
	if (!nframe) return *AnimPlay.CurrentFrame;

	float factor = AnimPlay.FramePoint / AnimPlay.CurrentFrame->Duration;

	Rectangle rect{
		lerp(cframe->Rect.x,nframe->Rect.x,factor),
		lerp(cframe->Rect.y,nframe->Rect.y,factor),
		lerp(cframe->Rect.width,nframe->Rect.width,factor),
		lerp(cframe->Rect.height,nframe->Rect.height,factor)
	};

	Rectangle source{
		lerp(cframe->Source.x,nframe->Source.x,factor),
		lerp(cframe->Source.y,nframe->Source.y,factor),
		lerp(cframe->Source.width,nframe->Source.width,factor),
		lerp(cframe->Source.height,nframe->Source.height,factor)
	};

	Vector2 pivot = {
		lerp(cframe->Pivot.x,nframe->Pivot.x,factor),
		lerp(cframe->Pivot.y,nframe->Pivot.y,factor)
	};

	AnimFrame interpframe;
	interpframe.Rect = rect;
	interpframe.Source = source;
	interpframe.Pivot = pivot;
	interpframe.Rotation = (cframe->Rotation + factor * (nframe->Rotation - cframe->Rotation));
	interpframe.color = ColorLerp(cframe->color, nframe->color, factor);

	return interpframe;
}
void AnimationScene::DrawAnimationFrame(Vector2 pos) {
	if (!CurrentData || CurrentData->Texture.id == 0 || !CurrentData->Frames.size()) return;
	AnimFrame frame = AnimPlay.CurrentFrame->Interpolate? GetCurrentFrameInterpolateData() : *AnimPlay.CurrentFrame;
	
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

	DrawTexturePro(CurrentData->Texture, frame.Source, destRect, pivot, frame.Rotation, frame.color);
}