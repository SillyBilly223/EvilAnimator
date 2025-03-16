
#include "Animation.h"

#include "stdlib.h"
#include "stdio.h"

AnimFrame Frame_Interpolate(AnimFrame cframe, AnimFrame sframe, float factor) {

	SprRect rect = {
		ANIMLERP(cframe.Rect.x,sframe.Rect.x,factor),
		ANIMLERP(cframe.Rect.y,sframe.Rect.y,factor),
		ANIMLERP(cframe.Rect.width,sframe.Rect.width,factor),
		ANIMLERP(cframe.Rect.height,sframe.Rect.height,factor)
	};
	SprRect source = {
		ANIMLERP(cframe.Source.x,sframe.Source.x,factor),
		ANIMLERP(cframe.Source.y,sframe.Source.y,factor),
		ANIMLERP(cframe.Source.width,sframe.Source.width,factor),
		ANIMLERP(cframe.Source.height,sframe.Source.height,factor)
	};
	SprVec2 pivot = {
		ANIMLERP(cframe.Pivot.x,sframe.Pivot.x,factor),
		ANIMLERP(cframe.Pivot.y,sframe.Pivot.y,factor)
	};

	AnimFrame lerpframe = {
		.Rect = rect,
		.Source = source,
		.Pivot = pivot,
		.Rotation = ANIMLERP(cframe.Rotation, sframe.Rotation, factor),
		.color = SprColorLerp(cframe.color, sframe.color, factor),
		.Duration = cframe.Duration,
		.Interpolate = cframe.Interpolate,
	};

	return lerpframe;
}

void Anim_SaveRawAnimationData(const char path) {
	FILE* file;
	if (fopen_s(&file, path, "wb") != 0) {
		printf("ANIMATION_SAVE: Failed to open file for saving");
		return;
	}

	const char* errorsmg = "";

	AnimSaveData data;

	if (!data.Image.data)
	{
		errorsmg = "ALLOCATE_ANIMIMAGEDATA"; goto FILEFAILSAVE;
	}

	if (!fwrite(&data, sizeof(AnimSaveData), 1, file))
	{
		errorsmg = "ANIMSAVEDATA"; goto FILEFAILSAVE;
	}
	
	for (int i = 0; i < data.AnimCount; i++) {
		if (!fwrite(&data.Animations[i], sizeof(Animation), 1, file)) {
			errorsmg = "ANIMATIONDATA"; goto FILEFAILSAVE;
		}
		if (!fwrite(&data.Animations[i].Frames, sizeof(AnimFrame), &data.Animations[i].FrameCount, file)) {
			errorsmg = "ANIMATIONFRAME"; goto FILEFAILSAVE;
		}
	}

	if (!fwrite(&data.Image, sizeof(SprImage), 1, file))
	{
		errorsmg = "ANIMIMAGEINFO"; goto FILEFAILSAVE;
	}

	if (!fwrite(data.Image.data, data.Image.width * data.Image.height * 4, 1, file))
	{
		errorsmg = "ANIMIMAGEDATA"; goto FILEFAILSAVE;
	}

	fclose(file);
	return;
FILEFAILSAVE:
	printf("ANIMATION_SAVE: Failed to save animation (%s)", errorsmg);
	fclose(file);
}

AnimSaveData* Anim_LoadRawAnimationData(const char path) {
	FILE* file;
	if (fopen_s(&file, path, "rb") != 0) {
		printf("ANIMATION: Invalid path %s", path);
		return NULL;
	}

	const char* errorsmg = "";

	AnimSaveData* data = malloc(sizeof(AnimSaveData));
	data->AnimCount = 0; data->Animations = NULL;

	if (!fread_s(data, sizeof(AnimSaveData), sizeof(AnimSaveData), 1, file))
	{
		errorsmg = "ANIMSAVEDATA"; goto FAILLOADFILE;
	}
	if (!data->AnimCount) { errorsmg = "NOANIMATIONS"; goto FAILLOADFILE; }

	data->Animations = malloc(sizeof(Animation) * data->AnimCount);
	if (!data->Animations) { errorsmg = "ALLOCATE_ANIMATION"; goto FAILLOADFILE; }

	for (int i = 0; i < data->AnimCount; i++) {
		/*
		if (!fwrite(&data.Animations[i], sizeof(Animation), 1, file)) {
			errorsmg = "ANIMATIONDATA"; goto FILEFAILSAVE;
		}
		if (!fwrite(&data.Animations[i].Frames, sizeof(AnimFrame), &data.Animations[i].FrameCount, file)) {
			errorsmg = "ANIMATIONFRAME"; goto FILEFAILSAVE;
		}
		*/
		//if (!fread_s())
		continue;
	FAILLOADANIM:
		for (int j = i-1; j >= 0; j--) {
			free(data->Animations[j].Frames);
		}
		free(data->Animations);
		goto FAILLOADFILE;
	}

	if (!fread_s(&data->Image, sizeof(SprImage), sizeof(SprImage), 1, file))
	{
		errorsmg = "ANIMIMAGEINFO"; goto FAILLOADFILE;
	}

	data->Image.data = malloc(data->Image.width * data->Image.height * 4);
	if (!data->Image.data) { errorsmg = "ALLOCATE_ANIMIMAGEDATA"; goto FAILLOADFILE; }
	if (!fread_s(data->Image.data, data->Image.width * data->Image.height * 4, data->Image.width * data->Image.height * 4, 1, file))
	{
		errorsmg = "ANIMIMAGEDATA"; goto FAILLOADFILE;
	}

	fclose(file);
	return data;
FAILLOADFILE:
	printf("ANIMATION_LOAD: Failed to load Animation (%s)", errorsmg);
	fclose(file);
	return NULL;
}

/*

AnimSaveData* Anim_LoadRawAnimationData(const char path) {
	FILE* file;
	if (fopen_s(&file, path, "rb") != 0) {
		printf("ANIMATION: Invalid path %s", path);
		return NULL;
	}

	const char* errorsmg = "";

	AnimSaveData* data = malloc(sizeof(AnimSaveData));
	data->FrameCount = 0; data->Frames = NULL;

	if (!fread_s(data, sizeof(AnimSaveData), sizeof(AnimSaveData), 1, file))
	{ errorsmg = "ANIMSAVEDATA"; goto FAILLOADFILE; }
	if (!data->FrameCount) { errorsmg = "FAILEDLOAD/NOFRAMES - FRAMECOUNT"; goto FAILLOADFILE; }

	data->Frames = malloc(sizeof(AnimFrame) * data->FrameCount);
	if (!data->Frames) { errorsmg = "ALLOCATE_ANIMFRAME"; goto FAILLOADFILE; }

	if (fread_s(data->Frames, sizeof(AnimFrame) * data->FrameCount, sizeof(AnimFrame), data->FrameCount, file) != data->FrameCount)
	{ errorsmg = "ANIMFRAMES"; goto FAILLOADFILE; }
	if (!fread_s(&data->Image, sizeof(SprImage), sizeof(SprImage), 1, file))
	{ errorsmg = "ANIMIMAGEINFO"; goto FAILLOADFILE; }

	data->Image.data = malloc(data->Image.width * data->Image.height * 4);
	if (!data->Image.data) { errorsmg = "ALLOCATE_ANIMIMAGEDATA"; goto FAILLOADFILE; }
	if (!fread_s(data->Image.data, data->Image.width * data->Image.height * 4, data->Image.width * data->Image.height * 4, 1, file))
	{ errorsmg = "ANIMIMAGEDATA"; goto FAILLOADFILE; }

	fclose(file);
	return data;
FAILLOADFILE:
	printf("ANIMATION: Failed to load Animation (%s)", errorsmg);
	fclose(file);
	return NULL;
}
AnimationData Anim_LoadAnimationData(const char path) {
	AnimationData animdata;
	AnimSaveData* dataraw = Anim_LoadRawAnimationData(path);
	if (dataraw == NULL) return animdata;

	animdata.CurrentTime = 0; animdata.FramePoint = 0; animdata.FrameIndex = 0;
	animdata.FrameCount = dataraw->FrameCount;
	animdata.Frames = dataraw->Frames;

	free(dataraw);

	return animdata;
}
void Anim_UnloadAnimationData(AnimationData animdata) {
	free(animdata.Frames); //Thats it :)
}

int Animation_Update(AnimationData* anim, double deltatime) {
	if (!anim->Playing) return;
	anim->CurrentTime += deltatime;
	anim->FramePoint += deltatime;
	if (anim->Frames[anim->FrameIndex].Duration <= anim->FramePoint) {
		if (anim->FrameIndex >= anim->FrameCount) {
			if (anim->DoseLoop) {
				Animation_Reset(anim);
			}
			else {
				anim->Playing = SPRFALSE;
			}
			return SPRTRUE;
		}
		anim->FrameIndex++;
		anim->FramePoint = Animation_GetFramePoint(anim);
	}
	return SPRFALSE;
}

float Animation_GetFramePoint(AnimationData* anim) {
	float timeoffset = 0;
	for (int i = 0; i < anim->FrameCount; i++) {
		if (anim->CurrentTime < timeoffset + anim->Frames[i].Duration) {
			return anim->CurrentTime - timeoffset;
		}
		timeoffset += anim->Frames[i].Duration;
	}
	return timeoffset;
}
float Animation_GetTimeFactor(AnimationData* anim) {
	return anim->FramePoint / anim->Frames[anim->FrameIndex].Duration;
}
float Animation_GetAnimLength(AnimationData* anim) {
	float combinetime = 0;
	for (int i = 0; i < anim->FrameCount; i++) {
		combinetime += anim->Frames[i].Duration;
	}
	return combinetime;
}

int Animation_GetFrameIndexAtPos(AnimationData* anim, float position) {
	if (position <= 0 || position > Animation_GetAnimLength(anim)) return 0;
	float combinetime = 0;
	for (int i = 0; i < anim->FrameCount; i++) {
		combinetime += anim->Frames[i].Duration;
		if (position <= combinetime) return i;
	}
	return 0;
}
AnimFrame* Animation_GetFrameAtPos(AnimationData* anim, float position) {
	if (position < 0 || position > Animation_GetAnimLength(anim)) return NULL;
	float combinetime = 0;
	for (int i = 0; i < anim->FrameCount; i++) {
		combinetime += anim->Frames[i].Duration;
		if (position <= combinetime) return &anim->Frames[i];
	}
	return NULL;
}

void Animation_SetPositon(AnimationData* anim, float position) {
	if (position < 0 || position > Animation_GetAnimLength(anim)) return;
	anim->CurrentTime = position;
	anim->FramePoint = Animation_GetFramePoint(anim);
	anim->FrameIndex = Animation_GetFrameIndexAtPos(anim, position);
}
void Animation_Reset(AnimationData* anim) {
	anim->CurrentTime = 0;
	anim->FramePoint = 0;
	anim->FrameIndex = 0;
}
*/