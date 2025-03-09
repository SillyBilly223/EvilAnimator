#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>
#include <stdint.h>

#define SPR_RZ { 0,0,0,0 }

typedef struct Sprite {
	Texture2D* Texture;
	Rectangle Source;
	Rectangle Rect;
	int16_t Rotation;
	Color Color;

	Sprite() :
		Texture(nullptr), 
		Source(SPR_RZ), Rect(SPR_RZ), 
		Rotation(0), Color({255,255,255,255}) {
	}
	Sprite(Texture2D* texture) :
		Texture(texture), 
		Source({0,0,(float)texture->width,(float)texture->height }), Rect({0,0,(float)texture->width,(float)texture->height }), 
		Rotation(0), Color({255,255,255,255}) {
	}
} Sprite;

/// <summary>
/// Draw Texture2D using Sprite Elements
/// </summary>
/// <param name="sprite"></param>
void DrawSprite(Sprite* sprite);

/// <summary>
/// Draw Texture2D using Sprite Elements
/// Uses Vector as Sprite Pos
/// </summary>
void DrawSpriteV(Sprite* sprite, Vector2 position);

/// <summary>
/// Draw Texture2D using Sprite Elements
/// Uses Rect as Sprite Base Pos
/// </summary>
void DrawSpriteRF(Sprite* sprite, Rectangle rect);

#endif 
