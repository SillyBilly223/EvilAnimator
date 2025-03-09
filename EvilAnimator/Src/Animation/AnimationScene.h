#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H

#include "Data/AnimationData.h"
#include "Data/Sprite.h"

#include <list>

struct TempAnimationData {
	Texture2D Texture = { 0 };
	std::list<AnimFrame> Frames;
};

struct SpriteInterface {
	bool Selected = false;
	bool DragX = false; bool DragY = false;

	Vector2 LastRectPos = { 0,0 };
	Vector2 LastMousePos = { 0,0 };
};

struct SpriteAnimPlay {
	bool Playing;
	float CurrentTime = 0;
	float FramePoint = 0;
	AnimFrame* CurrentFrame = nullptr;
};

enum SceneShow {
	SINF_ANIM,
	SINF_SPRITE,
};

struct AnimationScene {
public:
	TempAnimationData* CurrentData = nullptr;
	SpriteAnimPlay AnimPlay;	 
	SpriteInterface SprInterface;

	int FrameIndex = 0;
	enum SceneShow ShoweCase = SINF_ANIM;

	void UpdateSpriteHandling(Vector2 mpos, Vector2 dpos);

	void TickFrames();
	void UpdateFramePoint();
	void UpdateSelectedFrame();

	float GetAnimationTime();
	float GetCurrentFramePoint();

	AnimFrame* GetNextFrame();
	AnimFrame* GetCurrentFrame();
	std::list<AnimFrame>::iterator GetCurrentFrame_LI();

	void MoveCurrentFrame(int dist);

	AnimFrame GetCurrentFrameInterpolateData();

	void DrawAnimationFrame(Vector2 pos);
};

#endif 
