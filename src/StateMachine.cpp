#include "Statemachine.hpp"

#include <string>
#include <iostream>
using namespace std;

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "Console.hpp"
#include "EventManager.hpp"
#include "Map.hpp"

extern string  		app_version;
extern SDL_Renderer	*render;
extern SDL_Window	*window;
extern Console      *console;
extern EventManager	*event;
extern SDL_Rect     screen;

class Game
{
	private:
		bool loaded;

		Map		*map;
		Entity	*mob;


	public:

		Game()
		{
			this->loaded = false;

			this->map = NULL;
			this->mob = NULL;
		}

		~Game()
		{
			if (this->loaded)
				this->unload();
		}

		void load(void)
		{
			Tile::load_all_texture();
			Text::load_font();

			map = new Map(Map_lvl::TEST);
			mob = new Entity("Mob_random", "img/entity/mobTest.png");

			this->loaded = true;
		}

		void unload(void)
		{
			delete (map);
			delete (mob);
			
			Tile::unload_all_texture();
			Text::unload_font();
			
			this->map		= NULL;
			this->mob		= NULL;
			this->loaded	= false;
		}

		void proc(State *state)
		{
			if (!this->loaded)
				this->load();

			if (event->getKeyUp(SDL_SCANCODE_F3))
			{
				Printable::debug = !Printable::debug;
				console->log("debug = %s", (Printable::debug) ? "Enable" : "Disable");
			}

			if (mob->canMove())
			{
				SDL_Point futurePos = mob->getPosition();
				
				if (mob->canMove() && event->getKey(SDL_GetScancodeFromKey(SDLK_z)))
				{
					futurePos.y--;
					mob->move(Direction::NORTH, map->getTile(futurePos));
				}
				if (mob->canMove() && event->getKey(SDL_GetScancodeFromKey(SDLK_s)))
				{
					futurePos.y++;
					mob->move(Direction::SOUTH, map->getTile(futurePos));
				}
				if (mob->canMove() && event->getKey(SDL_GetScancodeFromKey(SDLK_q)))
				{
					futurePos.x--;
					mob->move(Direction::WEST, map->getTile(futurePos));
				}
				if (mob->canMove() && event->getKey(SDL_GetScancodeFromKey(SDLK_d)))
				{
					futurePos.x++;
					mob->move(Direction::EAST, map->getTile(futurePos));
				}
			}

			mob->proc();

			map->focus(mob->getPosition_screen());
			map->print();

			mob->print_onMap(map->getPosition());

			Printable::proc_debug();
		}
};

class Menu
{
	private:
	public:
		Menu()
		{

		}

		~Menu()
		{

		}

		void load(void)
		{

		}

		void proc(State *state)
		{

		}

};

Game *gameCnf = NULL;
Menu *menuCnf = NULL;

int init(State *state)
{
    int audio, mixer;
	audio = mixer = 0;

    screen.h = 800;
    screen.w = 1400;

	/* info CPU, RAM,... */
	int CPUcount = SDL_GetCPUCount();
	int CPUcache = SDL_GetCPUCacheLineSize();
	int RAMsize  = SDL_GetSystemRAM();

	printf("Hardware info\r\n");
	printf("\tCPU %d core %dMB L1 cache\r\n", CPUcount, CPUcache);
	printf("\t%dMB of RAM\r\n\r\n", RAMsize);

	/* Main SDL initialisation */
	window	= SDL_CreateWindow(app_version.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.w, screen.h, SDL_WINDOW_RESIZABLE);
	render	= SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	audio	= Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
	mixer	= Mix_Init(MIX_INIT_OGG);
	
    if (window == NULL) {
		printf("Error while init window\r\n");
        *state = EXIT;
		return (-1);
	}
	else {
		printf("Init window at 0x%x\r\n", (unsigned int) window);
	}
	if (render == NULL) {
		printf("Error while init render\r\n");
        *state = EXIT;
		return (-1);
	}
	else {
		printf("Init render at 0x%x\r\n", (unsigned int) render);
	}
	if (audio < 0) {
		printf("Error while init SDL audio\r\n");
        *state = EXIT;
		return (-1);
	}
	else {
		printf("Init SDL audio\r\n");
	}
	if (mixer == 0) {
		printf("Error while init SDL_Mixer audio\r\n");
        *state = EXIT;
		return (-1);
	}
	else {
		printf("Init SDL_Mixer audio\r\n");
	}

	printf("\r\n");

	SDL_GetWindowSize(window, &screen.w, &screen.h);


	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(render, &screen);
	SDL_RenderPresent(render);

	console = new Console(&screen);
	event   = new EventManager(state);

	gameCnf = new Game();
	menuCnf = new Menu();

    *state = MENU;

	console->log("Game loaded successfully");

    return (0);
}

int exit(State *state)
{
	console->log("Exit game ...");

	delete (gameCnf);
	delete (menuCnf);

    delete (console);
	delete (event);

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	printf("Exit successfull\r\n");

    return (0);
}

int menu(State *state)
{
	/* TODO : menu */
	*state = GAME;

/*
	if (menuCnf->loaded)
		menuCnf->proc(state);
	else
		menuCnf->load();
*/
    return (0);
}

int game(State *state)
{
	gameCnf->proc(state);

    return (0);
}

