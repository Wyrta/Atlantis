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

GameManager::~GameManager(void) {
    Mix_FreeMusic(ambiantMusic);
}

void GameManager::eventHandler(SDL_Window *window, SDL_Rect *screen, GUI *gui, DebugMenu *debugMenu, MapManager *map, Player *player) {
    //screen size
    SDL_GetWindowSize(window, &screen->w, &screen->h);   


    SDL_PollEvent(&event);
        buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
        if((buttons & SDL_BUTTON_LMASK) != 0) { /* left click */
            int x = 0; /* mouse pos */
            int y = 0; /* mouse pos */
            SDL_GetWindowPosition(window, &x, &y);

            if (this->L_btn == false)
            {
                this->L_btn = true;
                this->playClick();

                if (player->state != ATTACK)
                    player->isAttack = true;

                player->state = ATTACK; 
                player->attackCoo.x = mouse.x - map->rectMap.x;
                player->attackCoo.y = mouse.y - map->rectMap.y;

                if(player->attackCoo.x < 0) player->attackCoo.x = 0;
                if(player->attackCoo.y < 0) player->attackCoo.y = 0;
                if(player->attackCoo.x > map->rectMap.w) player->attackCoo.x = map->rectMap.w;
                if(player->attackCoo.y > map->rectMap.h) player->attackCoo.y = map->rectMap.h;
            }


        } 
        else
        {
            this->L_btn = false;
        }
        if((buttons & SDL_BUTTON_RMASK) != 0) { /* right click */
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
                this->playClick();
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

        if(keyboardState[SDL_SCANCODE_F]) { /* use */
            while (keyboardState[SDL_SCANCODE_F]) SDL_PollEvent(&event);  /* while F pressed do nothing and update event */
            if(map->isOnSign(player->hitbox) ) this->running = false;
            else player->interaction = true;
        }
        if(keyboardState[SDL_SCANCODE_SPACE]) {/* camera state : stick to player or not */
            while (keyboardState[SDL_SCANCODE_SPACE]) SDL_PollEvent(&event);  /* while space pressed do nothing and update event */
            swapBoolStates(&screenFollowPlayer);
        }
        if(keyboardState[SDL_SCANCODE_TAB]) { /* show/hide inventory */
            while (keyboardState[SDL_SCANCODE_TAB]) SDL_PollEvent(&event);  /* while space pressed do nothing and update event */
            swapBoolStates(&gui->inventoryState);
        }

        if(keyboardState[SDL_SCANCODE_F3]) { /* more info */
            while(keyboardState[SDL_SCANCODE_F3]) SDL_PollEvent(&event);  /* while F3 pressed do nothing and update event */
            swapBoolStates(&debugMenu->active);
        }
        if(keyboardState[SDL_SCANCODE_F11]) {   /* enable/disable fullscreen mode */
            while (keyboardState[SDL_SCANCODE_F11]) SDL_PollEvent(&event);  /* while F11 pressed do nothing and update event */
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
