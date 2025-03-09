#ifndef ANIMATION_DATA_H
#define ANIMATION_DATA_H

#include <raylib.h>
#include <stdint.h>

typedef struct AnimFrame {
	Rectangle Source;
	Rectangle Rect;
	Vector2 Pivot;
	int16_t Rotation;
	Color color;
	float Duration;
	bool Interpolate;
}AnimFrame;

typedef struct AnimationData {
	int FrameCount;
	Texture2D* Texture;
	AnimFrame* Frames;
}AnimationData;

#endif
