#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	u16* gfx;
	int color;
	int x, y;
} Sprite;

void createSquare(Sprite sprite, OamState* screen, int count);
int speed = 3;
Sprite spriteTouchUp = {0, ARGB16(1, 55, 12, 55), 10 , 10};
Sprite spriteTouchDown = {0, ARGB16(1, 55, 12, 55), 10, 10};
Sprite spriteKeyDown = {0, ARGB16(1, 31, 12, 12), 30, 30};

int main(int argc, char** argv){

	touchPosition touch;

	// u16* mainGFX = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp); // top screen square
	// u16* subGFX = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp); // bottom screen square

	//Set Upper Screen
	videoSetMode(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	oamInit(&oamMain, SpriteMapping_1D_128, false);

	//Set Bottom Screen
	videoSetModeSub(MODE_0_2D);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	
	oamInit(&oamSub, SpriteMapping_1D_128, false);

	spriteTouchUp.gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
	spriteTouchDown.gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);
	spriteKeyDown.gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);

	while(1){

		scanKeys();
		int key = keysHeld();

		switch(key) {
			case KEY_UP:
				spriteKeyDown.y -= speed;
				break;
			case KEY_DOWN:
				spriteKeyDown.y += speed;
				break;
			case KEY_RIGHT:
				spriteKeyDown.x += speed;
				break; 
			case KEY_LEFT:
				spriteKeyDown.x -= speed;
				break;
			case KEY_TOUCH:
				touchRead(&touch);
				spriteTouchUp.x = 255 - touch.px;
				spriteTouchUp.y = 190 - touch.py;
				spriteTouchDown.x = touch.px;
				spriteTouchDown.y = touch.py;
				break;
		}

		createSquare(spriteTouchUp, &oamMain, 1);
		createSquare(spriteTouchDown, &oamSub, 2);
		createSquare(spriteKeyDown, &oamSub, 3);

		swiWaitForVBlank(); // clean the screen
		oamUpdate(&oamMain); // (sub) updates the oam before so VBlank can update the graphics on screen
		oamUpdate(&oamSub); // (sub) updates the oam before so VBlank can update the graphics on screen
	}

	return 0;
}

void createSquare(Sprite sprite, OamState* screen, int count) {
	dmaFillHalfWords(sprite.color, sprite.gfx, 16*16*2); // this is how to assign the color fill to the oam gfx
	oamSet(screen, //is it upper screen of bottom?
		count, // the oam entry to set
		sprite.x, sprite.y, // where should be positioned (x,y)?
		0, // priority
		15, // palette for 16 color sprite or alpha for bmp sprite
		SpriteSize_16x16, // size
		SpriteColorFormat_Bmp, // color type
		sprite.gfx, // the oam gfx
		0, //affine index
		true, //double the size of rotated sprites
		false, //don't hide the sprite
		false, false, //vflip, hflip
		false //apply mosaic
		);
}

