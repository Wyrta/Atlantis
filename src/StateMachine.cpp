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
extern Mouse_t		mouse;


static void bt0_proc(Button *btn);

static void bp_quit(Button *btn);
static void bp_attack(Button *btn);
static void bp_spell0(Button *btn);
static void bp_spell1(Button *btn);
static void bp_spell2(Button *btn);
static void bp_spell3(Button *btn);

Entity *fighter_top;
Entity *fighter_bot;

class Game
{
	private:
		bool loaded;

		Map		*map;
		Player	*player;

		string mapName = "test";

		Button *btn;

	public:

		Game()
		{
			this->loaded	= false;

			this->map		= NULL;
			this->player	= NULL;
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
			NPC::load_history();

			this->map = new Map(mapName);
			NPC::load_all(mapName);

			this->player = new Player("player", "img/entity/player2.png");
			this->player->setAnimation(3, -1, {0, 0, 22, 22});

			this->btn = new Button({screen.w - 110, screen.h - 110, 100, 100}, Button_type::TXT, "Bouton");


			this->btn->onClick_func = bt0_proc;

			this->loaded = true;
		}

		void unload(void)
		{
			delete (this->map);
			delete (this->player);
			delete (this->btn);

			NPC::unload_all();
			Tile::unload_all_texture();
			Text::unload_font();
			
			this->map		= NULL;
			this->player	= NULL;
			this->btn		= NULL;

			this->loaded	= false;
		}

		void proc(State *state)
		{
			if (!this->loaded)
				this->load();

			if (event->getKeyUp(SDL_SCANCODE_F3))
			{
				Printable::debug = !Printable::debug;
				console->log("%s debug", (Printable::debug) ? "Enable" : "Disable");
			}

			if (mouse.wheel > 0)
			{
				// Printable::tilesize++;
				console->log("Zoom");
			}
			else if (mouse.wheel < 0)
			{
				// Printable::tilesize--;
				console->log("deZoom");
			}

			/* start fight */
			if (event->getKeyUp(SDL_GetScancodeFromKey(SDLK_f)))
			{
				/* toggle fight value */

				for (int i = 0; i < MAX_ENTITY; i++)
				{
					if ((Entity::allEntity[i] != NULL) && (this->player->canReach(Entity::allEntity[i])) )
					{
						*state		= State::FIGHT;
						fighter_top = Entity::allEntity[i];
						fighter_bot = this->player;

						break;
					}

				}
				

			}

			this->player->proc((int *)map);

			this->map->focus(player->getPosition_screen());

			Button::procAll();

			this->print();

			/* Reload game */
			if (event->getKeyUp(SDL_SCANCODE_F5))
			{
				console->log("Reload game");
				this->unload();
			}
		}

		void print(void)
		{
			SDL_Point mapPos = map->getPosition();
			/* Print Map */
			this->map->print();

			/* Print NPC */
			NPC::proc_print(mapPos);
			
			/* Print Player*/
			this->player->print_onMap(mapPos);

			Button::printAll(mapPos);
			
			/* Print Debug */
			Printable::proc_debug();
		}
};

class Menu
{
	private:
	/*
	gui_item
		-button
		-text
		-dfgdfgdsfg
	*/ 
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

		void print(void)
		{

		}
};

class Fight
{
	private:
		bool loaded;

		Button *spell[MAX_SPELL], *attack, *quit;
		Waifu *waifu_top, *waifu_bot;
		string atkReport_top, atkReport_bot;


	public:
		bool is_quit, is_attack;

		Fight()
		{
			this->loaded = false;
			this->waifu_top = NULL;
			this->waifu_bot = NULL;

			this->spell[0]	= NULL;
			this->spell[1]	= NULL;
			this->spell[2]	= NULL;
			this->spell[3]	= NULL;
			this->quit		= NULL;
			this->attack	= NULL;
		}

		~Fight()
		{

		}

		void load(void)
		{
			this->loaded	= true;
			this->is_quit	= false;
			this->is_attack	= false;

			/* load buttons */

			SDL_Rect buttonSize;
				buttonSize.w = 128;
				buttonSize.h = 32;

			buttonSize.x = screen.x + screen.w - 140*2;
			buttonSize.y = screen.y + screen.h - 40*2;

			attack = new Button(buttonSize, Button_type::TXT, "Attack");
			attack->onClick_func = bp_attack;
			
			buttonSize.y += 40;
			quit = new Button(buttonSize, Button_type::TXT, "Quit");
			quit->onClick_func = bp_quit;

			buttonSize.x = screen.x + screen.w - 140*2;
			buttonSize.y = screen.y + screen.h - 40*2;

			spell[0] = new Button(buttonSize, Button_type::TXT, "Spell 0");
			buttonSize.x += 140;
			spell[1] = new Button(buttonSize, Button_type::TXT, "Spell 1");
			buttonSize.y += 40;
			buttonSize.x -= 140;
			spell[2] = new Button(buttonSize, Button_type::TXT, "Spell 2");
			buttonSize.x += 140;
			spell[3] = new Button(buttonSize, Button_type::TXT, "Spell 3");
			
			spell[0]->onClick_func = bp_spell0;
			spell[1]->onClick_func = bp_spell1;
			spell[2]->onClick_func = bp_spell2;
			spell[3]->onClick_func = bp_spell3;

			spell[0]->enable	= false;
			spell[1]->enable	= false;
			spell[2]->enable	= false;
			spell[3]->enable	= false;

			quit->enable		= false;
			attack->enable		= false;

		}

		void unload(void)
		{
			this->loaded	= false;
			this->is_quit	= false;
			this->is_attack	= false;

			delete (attack);
			delete (quit);
			for (int i = 0; i < MAX_SPELL; i++)
				delete (spell[i]);
		}

		void proc(State *state)
		{
			if (!this->loaded)
				this->load();

			if (this->is_attack)
			{
				spell[0]->enable	= true;
				spell[1]->enable	= true;
				spell[2]->enable	= true;
				spell[3]->enable	= true;

				quit->enable		= false;
				attack->enable		= false;
			}
			else
			{
				spell[0]->enable	= false;
				spell[1]->enable	= false;
				spell[2]->enable	= false;
				spell[3]->enable	= false;

				quit->enable		= true;
				attack->enable		= true;
			}

			Button::procAll();

			this->print();

			/* load waifu if needed */
			this->waifu_top = fighter_top->getWaifu();
			this->waifu_bot = fighter_bot->getWaifu();

			if ((this->waifu_top == NULL) || (this->waifu_bot == NULL))
			{
				this->procFinish(state);
				return;
			}

			for (int i = 0; i < MAX_SPELL; i++)
			{
				if (spell[i]->pressed() )
				{
					this->atkReport_bot = this->waifu_bot->attack(this->waifu_top, i);
					this->atkReport_top = this->waifu_top->attack(this->waifu_bot, i);
					break;
				}
			}
			

			if (this->is_quit)
			{
				this->unload();
				*state = State::GAME;
				return;
			}
		}

		void print(void)
		{
			for (int i = 0; i < MAX_SPELL; i++)
				spell[i]->print_onMap();

			attack->print_onMap();
			quit->print_onMap();

			this->waifu_top->print();
			this->waifu_bot->print();
		}

		void procFinish(State *state)
		{
			/* jsp encore */
			*state = State::GAME;
		}
};




Game *gameCnf = NULL;
Menu *menuCnf = NULL;
Fight *fightCnf = NULL;

int init(State *state)
{
    int audio, mixer;
	audio = mixer = 0;

    screen.h = 600;
    screen.w = 800;

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
	
    if (window == NULL)
	{
		printf("Error while init window\r\n");
        *state = EXIT;
		return (-1);
	}
	else
	{
		printf("Init window at 0x%x\r\n", (unsigned int) window);
	}
	if (render == NULL)
	{
		printf("Error while init render\r\n");
        *state = EXIT;
		return (-1);
	}
	else
	{
		printf("Init render at 0x%x\r\n", (unsigned int) render);
	}
	if (audio < 0)
	{
		printf("Error while init SDL audio\r\n");
	}
	else
	{
		printf("Init SDL audio\r\n");
	}
	if (mixer == 0)
	{
		printf("Error while init SDL_Mixer audio\r\n");
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

	console  = new Console(&screen);
	event    = new EventManager(state);

	gameCnf  = new Game();
	menuCnf  = new Menu();
	fightCnf = new Fight();

    *state = MENU;

	console->log("Game loaded successfully");

	Waifu::load_all();

    return (0);
}

int exit(State *state)
{
	Waifu::unload_all();

	console->log("Exit game ...");

	delete (gameCnf);
	delete (menuCnf);
	delete (fightCnf);

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

int fight(State *state)
{
	fightCnf->proc(state);

    return (0);
}


void bp_quit(Button *btn)
{
	console->log("Proc button quit");
	fightCnf->is_quit = true;
}

void bp_attack(Button *btn)
{
	console->log("Proc button atk");
	fightCnf->is_attack = true;
}


void bt0_proc(Button *btn)
{
	console->log("Button %d cliked !!! ", btn->i_btn);
	fightCnf->is_attack = false;
}

void bp_spell0(Button *btn)
{
	console->log("Proc button spell 0");
	fightCnf->is_attack = false;
}

void bp_spell1(Button *btn)
{
	console->log("Proc button spell 1");
	fightCnf->is_attack = false;
}

void bp_spell2(Button *btn)
{
	console->log("Proc button spell 2");
	fightCnf->is_attack = false;
}

void bp_spell3(Button *btn)
{
	console->log("Proc button spell 3");
	fightCnf->is_attack = false;
}

