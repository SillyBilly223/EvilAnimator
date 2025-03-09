
#include "Sprite.h"

void DrawSprite(Sprite* sprite)
{
	DrawTexturePro(*sprite->Texture, sprite->Source, sprite->Rect, { 0, 0 }, sprite->Rotation, sprite->Color);
}
void DrawSpriteV(Sprite* sprite, Vector2 position)
{
	DrawTexturePro(*sprite->Texture, sprite->Source, { position.x, position.y, sprite->Rect.width, sprite->Rect.height }, { 0, 0 }, sprite->Rotation, sprite->Color);
}
void DrawSpriteRF(Sprite* sprite, Rectangle rect) {
	DrawTexturePro(*sprite->Texture, sprite->Source, { rect.x + sprite->Rect.x, rect.y + sprite->Rect.y, rect.width + sprite->Rect.width, rect.height + sprite->Rect.height }, { 0, 0 }, sprite->Rotation, sprite->Color);
}