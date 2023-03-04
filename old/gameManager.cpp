#include "gameManager.hpp"

extern SDL_Renderer *render;

GameManager::GameManager(string musicName) {
	running = true;

	/* Event setup */
	buttons = SDL_GetMouseState(&mouse.x, &mouse.y);

	/* Auto follow player */
	screenFollowPlayer = true;

	/* Music setup */
	ambiantMusic = Mix_LoadMUS(("./sound/music" + musicName + ".mp3").c_str());
	Mix_VolumeMusic(32);

	clickSound[0] = Mix_LoadWAV("./sound/noise/Click1.wav");
	clickSound[1] = Mix_LoadWAV("./sound/noise/Click2.wav");
}

GameManager::~GameManager(void)
{
	Mix_FreeMusic(ambiantMusic);
}

void GameManager::eventHandler(SDL_Window *window, SDL_Rect *screen, GUI *gui, DebugMenu *debugMenu, MapManager *map, Player *player)
{
	static bool key_F, key_SPACE, key_TAB, key_F3, key_F11;

	//screen size
	SDL_GetWindowSize(window, &screen->w, &screen->h);   

	SDL_PollEvent(&event);
		buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
		if((buttons & SDL_BUTTON_LMASK) != 0) /* left click */
		{
			int x = 0; /* mouse pos */
			int y = 0; /* mouse pos */
			SDL_GetWindowPosition(window, &x, &y);

			if (this->L_btn == false)
			{
				this->L_btn = true;
//				this->playClick();
				player->tryAttack((SDL_Point){mouse.x - map->rectMap.x, mouse.y - map->rectMap.y});
			}
		} 
		else
		{
			this->L_btn = false;
		}

		if((buttons & SDL_BUTTON_RMASK) != 0) /* right click */
		{
			int x = 0; /* mouse pos */
			int y = 0; /* mouse pos */
			SDL_GetWindowPosition(window, &x, &y);

			targetPosition.x = mouse.x - map->rectMap.x;
			targetPosition.y = mouse.y - map->rectMap.y;

			if(targetPosition.x < 0) targetPosition.x = 0;
			if(targetPosition.y < 0) targetPosition.y = 0;
			if(targetPosition.x > map->rectMap.w) targetPosition.x = map->rectMap.w;
			if(targetPosition.y > map->rectMap.h) targetPosition.y = map->rectMap.h;

			if (this->R_btn == false)
			{
				this->R_btn = true;
//				this->playClick();
				if (player->getState() == ATTACK)
				{
					player->setState(IDLE);
					player->setFrame(0);
				}
			}

		}
		else 
		{
			this->R_btn = false;
		}

		if(keyboardState[up_key   ] != 0 ) map->rectMap.y += 10;
		if(keyboardState[down_key ] != 0 ) map->rectMap.y -= 10;
		if(keyboardState[left_key ] != 0 ) map->rectMap.x += 10;
		if(keyboardState[right_key] != 0 ) map->rectMap.x -= 10;

		/* use */
		if(keyboardState[SDL_SCANCODE_F] > 0 && key_F == false)
		{ 
			if(map->isOnSign(player->getHitbox()) ) 
				this->running = false;
			else 
				player->interaction = true;

			key_F = true;
		} else if (keyboardState[SDL_SCANCODE_F] == 0 && key_F == true) 
		{
			key_F = false;
		}

		/* camera getState() : stick to player or not */
		if(keyboardState[SDL_SCANCODE_SPACE] > 0 && key_SPACE == false)
		{ 
			swapBoolStates(&screenFollowPlayer);
			key_SPACE = true;
		} else if (keyboardState[SDL_SCANCODE_SPACE] == 0 && key_SPACE == true) 
		{
			key_SPACE = false;
		}

		/* show/hide inventory */
		if(keyboardState[SDL_SCANCODE_TAB] > 0 && key_TAB == false)
		{
			swapBoolStates(&gui->inventoryState);
			key_TAB = true;
		} else if (keyboardState[SDL_SCANCODE_TAB] == 0 && key_TAB == true) 
		{
			key_TAB = false;
		}

		/* more info */
		if(keyboardState[SDL_SCANCODE_F3] > 0 && key_F3 == false)
		{
			swapBoolStates(&debugMenu->active);
			key_F3 = true;
		} else if (keyboardState[SDL_SCANCODE_F3] == 0 && key_F3 == true) 
		{
			key_F3 = false;
		}

		 /* enable/disable fullscreen mode */
		if(keyboardState[SDL_SCANCODE_F11] > 0 && key_F11 == false)
		{
			if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
				screen->h = 800;
				screen->w = 1400;
				SDL_SetWindowSize(window, screen->w, screen->h);
				SDL_SetWindowFullscreen(window, 0);
			} else {
				/* set new window size */
				if (SDL_GetDesktopDisplayMode(0, &screenSize) != 0) {   /* get display size if error render will be in 720p */
					cout << "Error fullscreen : " << SDL_GetError() << endl;
					screen->h = 720;
					screen->w = 1280;
				} else {
					screen->h = screenSize.h;
					screen->w = screenSize.w;
				}
				/* change window size */

				SDL_SetWindowSize(window, screen->w, screen->h);
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}

			key_F11 = true;
		} else if (keyboardState[SDL_SCANCODE_F11] == 0 && key_F11 == true) 
		{
			key_F11 = false;
		}

		if(keyboardState[SDL_SCANCODE_ESCAPE] ) running = false;
		if(event.type == SDL_QUIT) running = false;
	

}

void GameManager::startMusic(void) {
	Mix_PlayMusic(ambiantMusic, -1);
}

void GameManager::stopMusic(void) {
	Mix_ResumeMusic();
}

void GameManager::setMusicVolume(int volume) {
	if(volume > 128) volume = 128;
	if(volume < 0) volume = 0;

	Mix_VolumeMusic(volume);
}

void GameManager::playClick(void)
{
	int click_idx = intAleat(0, 1);

	Mix_PlayChannel(4, clickSound[click_idx], 0);
}

void GameManager::fpsLimiter(void)
{
	int delay_ms = 1000 / this->fps;
	this->renderTime = SDL_GetTicks() - this->lastFrameTicks;

	int ticks = delay_ms - (SDL_GetTicks() - this->lastFrameTicks);
	if (ticks <= 0) 
		ticks = 1;

	SDL_Delay(ticks);

	this->lastFrameTicks = SDL_GetTicks();
}


LoadingBar::LoadingBar(SDL_Point pos, int lgth, int perc)
{
	this->percentage = 0;
	
	this->addPercentage(perc);

	this->set_position(pos);
	this->set_length(lgth);
}

LoadingBar::~LoadingBar()
{
	return;
}

void LoadingBar::set_position(SDL_Point pos)
{
	this->background.x = pos.x;
	this->background.y = pos.y;

	this->bar.x = pos.x + 5;
	this->bar.y = pos.y + 5;
}

void LoadingBar::set_length(int lgth)
{
	this->length = lgth;
	
	this->background.h = 30;
	this->background.w = this->length;

	this->bar.h = 20;
	this->bar.w = ((float)(length - 5) / 100) * (float)this->percentage;
	if (this->bar.w > this->background.w)
		this->bar.w = this->background.w - 10;
}

void LoadingBar::addPercentage(int perc)
{
	this->percentage += perc;

	if (this->percentage > 100)
		this->percentage = 100;

	if (this->percentage < 0)
		this->percentage = 0;

	this->bar.w = ((float)(this->length - 5) / 100) * (float)this->percentage;
	if (this->bar.w > this->background.w)
		this->bar.w = this->background.w - 10;
}

void LoadingBar::print(void)
{
	SDL_SetRenderDrawColor(render, 100, 100, 100, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(render, &this->background);

	SDL_SetRenderDrawColor(render, 25, 25, 200, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(render, &this->bar);
}
