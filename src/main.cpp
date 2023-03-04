/**
 * Author : Wyrta
 * Aim : an rpg
 * License : Sills gaming
 * Date : 25/05/2021 - abort
 *		10/03/2022
 *
 */

#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "Console.hpp"
#include "Statemachine.hpp"
#include "EventManager.hpp"

#include "Printable.hpp"


using namespace std;

string  		app_version		= "Atlantis v23w09";
SDL_Window		*window 		= NULL;
SDL_Renderer	*render 		= NULL;
Console			*console 		= NULL;
EventManager	*event			= NULL;
SDL_Rect		screen;
Mouse_t			mouse;

//EventManager	*eventManager	= NULL;

int main(int argc, char *argv[])
{
	printf("Starting %s ...\r\n\r\n", app_version.c_str());

	/* random init */
	srand(0);

	/* SDL init */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);

	
	/*main loop*/

	State state = INIT;
	init(&state);

	while (state != EXIT) {
		SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(render, &screen);
		event->pollEvent();

		switch (state)
		{
			case INIT: state = MENU; break;
			case GAME: menu(&state); break;
			case MENU: game(&state); break;
			case EXIT: break;
			
			default: state = EXIT; break;
		}

		SDL_RenderPresent(render);

		/* TODO : wait for ticks */
		SDL_Delay(10);
	}

	exit(&state);
	
//	delete eventManager;
	SDL_Quit();
	return (0);
}