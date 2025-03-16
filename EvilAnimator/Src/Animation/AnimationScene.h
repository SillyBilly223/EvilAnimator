#ifndef ANIMSCENE_H
#define ANIMSCENE_H

#include "raylib.h"
#include "Data/Sprite.h"
#include "Data/Animation.h"

#include <list>

#define MAX_ANIM_NLEN 36

enum SceneShow {
	SINF_ANIM,
	SINF_FRAME,
	SINF_SPRITE,
	SINF_SETTINGS
};

struct TempAnimation {
	char* AnimName;
	std::list<AnimFrame> Frames;

	float GetAnimLength();
	float GetTimePointByIndex(int index);

	void ShiftFrame(int index, int dist);

	AnimFrame* GetFrameByIndex(int index);
	std::list<AnimFrame>::iterator GetFrameLIByIndex(int index);

	int GetFrameIndexAtTimePoint(float time);
};

struct TempAnimationData {
	Texture2D Texture = { 0 };
	std::list<TempAnimation> Animations;

	TempAnimation* GetAnimByIndex(int index);
};

struct SpriteInterface {
	bool Selected = false;
	bool DragX = false; bool DragY = false;

	Vector2 LastRectPos = { 0,0 };
	Vector2 LastMousePos = { 0,0 };
};

struct SpriteAnimPlay {
	bool Playing = false;

	bool OnionSkin = true;
	int OnionSkin_Depth = 1;

	float CurrentTime = 0;
	float FramePoint = 0;

	int AnimIndex = 0;
	int FrameIndex = 0;

	TempAnimation* CurrentAnim = nullptr;

	void TickAnimation();
	void UpdateAnimation();

	float GetFramePoint();
	AnimFrame* GetCurrentFrame();

	void DrawOnionFrames(Texture2D tex, Vector2 pos);
	void DrawAnimationFrame(Texture2D tex, Vector2 pos);
};

struct AnimationScene {
	TempAnimationData* CurrentData = nullptr;
	SpriteAnimPlay AnimPlay;	 
	SpriteInterface SprInterface;

	enum SceneShow ShoweCase = SINF_ANIM;

	int CellIndex = 0;
	std::list<SprRect> SprCells;

    void UnloadScene();
	void LoadScene(AnimSaveData data);

	void ConvertCurrentSceneToAnimData(AnimSaveData* data);

	void UpdateSpriteHandling(Vector2 mpos, Vector2 dpos);

	bool CanDisplayAnim();
	bool CanDisplayFrames();
	bool CanDisplayData();
};

#endif 
