#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

#define SPR_RZ { 0,0,0,0 }

//R8G8B8A8 (32bit)
typedef struct SprColor {
	unsigned char r;        // Color red value
	unsigned char g;        // Color green value
	unsigned char b;        // Color blue value
	unsigned char a;        // Color alpha value
} SprColor;

typedef struct SprVec2 {
	float x;
	float y;
} SprVec2;
typedef struct SprRect {
	float x;                // Rectangle top-left corner position x
	float y;                // Rectangle top-left corner position y
	float width;            // Rectangle width
	float height;           // Rectangle height
} SprRect;

typedef struct SprImage {
	void* data;             // Image raw data
	int width;              // Image base width
	int height;             // Image base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} SprImage;

typedef char SprBool;
#define SPRFALSE (0)
#define SPRTRUE (1)

#define SPRCOL_WHITE ((SprColor){ 255,255,255,255 })

typedef struct Sprite {
	void* Texture;
	SprRect Source;
	SprRect Rect;
	SprVec2 Pivot;
	int Rotation;
	SprColor color;
} Sprite;

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

SprColor SprColorLerp(SprColor color1, SprColor color2, float factor);


#ifdef RAYLIB_H

void RL_DrawSprite(Sprite* sprite);
void RL_DrawSpriteV(Sprite* sprite, Vector2 position);
void RL_DrawSpriteRF(Sprite* sprite, Rectangle rect);

#endif

#if defined(__cplusplus)
}
#endif

#endif 
