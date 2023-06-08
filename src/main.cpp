#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "Console.hpp"
#include "Statemachine.hpp"
#include "EventManager.hpp"

using namespace std;

string  		app_version		= "Atlantis v23w13";
SDL_Window		*window 		= NULL;
SDL_Renderer	*render 		= NULL;
Console			*console 		= NULL;
EventManager	*event			= NULL;
SDL_Rect		screen;
Mouse_t			mouse;
int 			i_tick			= 0;

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

	Uint32	lastTicks		= SDL_GetTicks();
	Uint32	currentTicks	= SDL_GetTicks();
	int 	syncTick		= 0;

	while (state != EXIT)
	{
		SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(render, &screen);
		event->pollEvent();

		switch (state)
		{
			case INIT:  state = MENU;  break;
			case GAME:  game(&state);  break;
			case FIGHT: fight(&state); break;
			case MENU:  menu(&state);  break;
			case EXIT:  break;
			
			default: state = EXIT; break;
		}

		SDL_RenderPresent(render);

		/* ticks management */
		currentTicks = SDL_GetTicks();
		syncTick = 10 - (currentTicks - lastTicks);
		if (syncTick < 0)
			console->log(log_t::ERROR, "Ticks num %d missed (last tick was %dms ago)", i_tick, currentTicks - lastTicks);
		else
			SDL_Delay(syncTick);

		lastTicks = SDL_GetTicks();
		i_tick++;
	}

	exit(&state);
	
//	delete eventManager;
	SDL_Quit();
	return (0);
}