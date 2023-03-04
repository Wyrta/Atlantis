#include "GUI.hpp"

extern SDL_Renderer *render;

GUI::GUI(SDL_Rect screen) {
	//load texture for the follows player check button
	checkBP = createTexture(render, &rectCheckBP, "./img/checkBP.png");
	rectCheckBP.x = 0; rectCheckBP.y = 0; rectCheckBP.w /=2; 
	rectCheckBPonScreen.x = screen.w-75; rectCheckBPonScreen.y = screen.h-75; rectCheckBPonScreen.h = rectCheckBP.h; rectCheckBPonScreen.w = rectCheckBP.w;

	//load inventory data
	textureInventory = createTexture(render, &rectInventory, "./img/inventory.png");
	rectInventory.x = 0; rectInventory.y = 0;
	scratchsound = Mix_LoadWAV("./sound/noise/paperScratch.ogg");
	scratchsound->volume = 32;


}
GUI::~GUI() { 
	SDL_DestroyTexture(checkBP); 
	SDL_DestroyTexture(textureInventory); 
	Mix_FreeChunk(scratchsound);
}

void GUI::followPlayer(SDL_Rect screen, bool isCheck) {
	if(isCheck) {
		rectCheckBP.x = 50; rectCheckBP.y = 0;
		rectCheckBPonScreen.x = screen.w-75; rectCheckBPonScreen.y = screen.h-75;
	} else {
		rectCheckBP.x = 0; rectCheckBP.y = 0;
		rectCheckBPonScreen.x = screen.w-75; rectCheckBPonScreen.y = screen.h-75;
	}
	SDL_RenderCopy(render, checkBP, &rectCheckBP, &rectCheckBPonScreen);
}

void GUI::inventory(SDL_Rect screen, Player *player, bool isActive) {

	if(oldInventoryState != isActive) {
		Mix_PlayChannel(2, scratchsound, 0);
		oldInventoryState = isActive;
	}

	if(isActive) SDL_RenderCopy(render, textureInventory, NULL, NULL);
}