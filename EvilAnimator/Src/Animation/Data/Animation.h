#ifndef ANIMATION_DATA_H
#define ANIMATION_DATA_H

#include "Sprite.h"

typedef struct AnimFrame {
	SprRect Source;
	SprRect Rect;
	SprVec2 Pivot;
	int Rotation;
	SprColor color;
	float Duration;
	SprBool Interpolate;
}AnimFrame;

typedef struct Animation {
	char* Name;
	unsigned int FrameCount;
	AnimFrame* Frames;
} Animation;

typedef struct AnimatorData {
	Sprite* Sprite;
	float CurrentTime;
	float FramePoint;
	int FrameCount;
	int FrameIndex;

	SprBool Playing;
	SprBool DoseLoop;

	Animation* Animations;
}AnimatorData;

typedef struct AnimSaveData {
	unsigned int AnimCount;
	Animation* Animations;
	SprImage Image;
} AnimSaveData;

#define ANIMLERP(a,b,f) (a + f * (b - a))

#if defined(__cplusplus)
extern "C" {
#endif

	AnimFrame Frame_Interpolate(AnimFrame cframe, AnimFrame sframe, float factor);

	void Anim_SaveRawAnimationData(const char path);
	AnimSaveData* Anim_LoadRawAnimationData(const char path);
	AnimatorData Anim_LoadAnimationData(const char path);
	void Anim_UnloadAnimationData(AnimatorData animdata);

	int Animation_Update(AnimatorData* Anim, double deltatime);

	float Animation_GetFramePoint(AnimatorData* anim);
	float Animation_GetTimeFactor(AnimatorData* anim);
	float Animation_GetAnimLength(AnimatorData* anim);

	int Animation_GetFrameIndexAtPos(AnimatorData* anim, float position);
	AnimFrame* Animation_GetFrameAtPos(AnimatorData* anim, float position);

	void Animation_SetPositon(AnimatorData* anim, float position);
	void Animation_Reset(AnimatorData* anim);

#if defined(__cplusplus)
}
#endif

#endif
