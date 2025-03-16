
#include "Sprite.h"

SprColor SprColorLerp(SprColor color1, SprColor color2, float factor)
{
	SprColor color = { 0 };

	if (factor < 0.0f) factor = 0.0f;
	else if (factor > 1.0f) factor = 1.0f;

	color.r = (unsigned char)((1.0f - factor) * color1.r + factor * color2.r);
	color.g = (unsigned char)((1.0f - factor) * color1.g + factor * color2.g);
	color.b = (unsigned char)((1.0f - factor) * color1.b + factor * color2.b);
	color.a = (unsigned char)((1.0f - factor) * color1.a + factor * color2.a);

	return color;
}


#ifdef RAYLIB_H 

void RL_DrawSprite(Sprite* sprite)
{
	DrawTexturePro(
		*(Texture2D*)sprite->Texture, *(Rectangle*)&sprite->Source, *(Rectangle*)&sprite->Rect,
		*(Vector2*)&sprite->Pivot, sprite->Rotation, *(Color*)&sprite->Color
	);
}
void RL_DrawSpriteV(Sprite* sprite, Vector2 position)
{
	DrawTexturePro(
		*(Texture2D*)sprite->Texture, *(Rectangle*)&sprite->Source, (Rectangle) { position.x, position.y, sprite->Rect.width, sprite->Rect.height },
		* (Vector2*)&sprite->Pivot, sprite->Rotation, * (Color*)&sprite->Color
	);
}
void RL_DrawSpriteRF(Sprite* sprite, Rectangle rect) {
	DrawTexturePro(
		*(Texture2D*)sprite->Texture, *(Rectangle*)&sprite->Source,
		(Rectangle) {
		rect.x + sprite->Rect.x, rect.y + sprite->Rect.y, rect.width + sprite->Rect.width, rect.height + sprite->Rect.height
	},
		* (Vector2*)&sprite->Pivot, sprite->Rotation, * (Color*)&sprite->Color
	);
}

#endif 
