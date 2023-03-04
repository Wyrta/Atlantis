/**
 * Author : Wyrta
 * Aim : an rpg
 * License : Sills gaming
 * Date : 25/05/2021 - abort
 *		10/03/2022
 * 
*/

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "map.hpp"
#include "console.hpp"
#include "eventManager.hpp"


#define ATLANTIS_VER "Atlantis v0.5.0-indev"

using namespace std;


SDL_Renderer	*render = NULL;
SDL_Window	 	*window = NULL;
Console			*console;
EventManager	*eventManager;

void task1(string msg);


int main(int argc, char *argv[]) {	
	cout << "Init game utils : ";

	srand(0);   //rand init
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  //SDL init 
	IMG_Init(IMG_INIT_PNG);					 //
	TTF_Init();								 //
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);	  //

	/**
	 * Channel used :
	 *  1 => Entity.follow()		 => footstep
	 *  2 => GUI.inventory()		 => scratchsound
	 *  3 => Npc.talk()			  => talkingSound
	 *  4 => gameManager.playClick() => click
	 * 
	*/

	/* Init window and render */

//	cout << "Init window and render" << endl;

	/* Init screen size */

	SDL_Rect screen;
		screen.h = 800;
		screen.w = 1400;
		screen.x = 0;
		screen.y = 0;

	/* init window & render */

	window = SDL_CreateWindow(ATLANTIS_VER, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.w, screen.h, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	/* Test window and render */
	if(window == NULL) {cout << "error on window" << endl; SDL_Delay(1000); return -1;}
	if(render == NULL) {cout << "error on render" << endl; SDL_Delay(1000); return -1;}

	SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(render, &screen);

	LoadingBar bar({ screen.w/4, 2*(screen.h/3)}, (screen.w/2), 25);
	bar.print();

	SDL_RenderPresent(render);

	SDL_Texture *splitScreen_texture = createTexture(render, NULL, "./img/splitScreen.png");
	SDL_RenderCopy(render, splitScreen_texture, NULL, NULL);

	bar.addPercentage(20);
	bar.print();

	SDL_RenderPresent(render);

	/* Init audio */

	if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0 ) { cout << "Error audio SDL" << endl; }
	if( Mix_Init(MIX_INIT_OGG) == 0) { cout << "Error SDL_mixer" << endl; }

	bar.addPercentage(20);
	bar.print();
	SDL_RenderPresent(render);

	/* console */
	console = new Console(&screen);

	/* eventManager */
	eventManager = new EventManager();

	/*main loop*/


	string running = "plage_00";

	while (running != "none") {
		running = map(&screen, running);
		
		SDL_RenderCopy(render, splitScreen_texture, NULL, NULL);
//		SDL_RenderPresent(render);
	}

	delete console;
	delete eventManager;

	SDL_DestroyTexture(splitScreen_texture);

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}