#include "gameManager.hpp"

GameManager::GameManager(string musicName) {
    running = true;

    //Event setup
    buttons = SDL_GetMouseState(&mouse.x, &mouse.y);

    //Music setup
    ambiantMusic = Mix_LoadMUS(("./sound/" + musicName + ".mp3").c_str());
    Mix_VolumeMusic(32);

}
GameManager::~GameManager() {
    Mix_FreeMusic(ambiantMusic);
}


void GameManager::eventHandler(SDL_Window *window, SDL_Rect *screen, GUI *gui, DebugMenu *debugMenu, MapManager *map, Player *player) {
    //screen size
    SDL_GetWindowSize(window, &screen->w, &screen->h);   

    
    SDL_PollEvent(&event);
        buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
        if((buttons & SDL_BUTTON_RMASK) != 0) {
            int x = 0;
            int y = 0;
            SDL_GetWindowPosition(window, &x, &y);
            targetPosition.x = mouse.x - map->rectMap.x;
            targetPosition.y = mouse.y - map->rectMap.y;

            if(targetPosition.x < 0) targetPosition.x = 0;
            if(targetPosition.y < 0) targetPosition.y = 0;
            if(targetPosition.x > map->rectMap.w) targetPosition.x = map->rectMap.w;
            if(targetPosition.y > map->rectMap.h) targetPosition.y = map->rectMap.h;
        }
        if(keyboardState[up_key   ] != 0 ) map->rectMap.y += 10;
        if(keyboardState[down_key ] != 0 ) map->rectMap.y -= 10;
        if(keyboardState[left_key ] != 0 ) map->rectMap.x += 10;
        if(keyboardState[right_key] != 0 ) map->rectMap.x -= 10;

        if(keyboardState[SDL_SCANCODE_F]) { //use 
            while (keyboardState[SDL_SCANCODE_F]) SDL_PollEvent(&event);  //while F pressed do nothing and update event
            if(map->isOnSign(player->hitbox) ) this->running = false;
        }
        if(keyboardState[SDL_SCANCODE_SPACE]) {//camera state : stick to player or not
            while (keyboardState[SDL_SCANCODE_SPACE]) SDL_PollEvent(&event);  //while space pressed do nothing and update event
            swapBoolStates(&screenFollowPlayer);
        }
        if(keyboardState[SDL_SCANCODE_TAB]) { //show/hide inventory
            while (keyboardState[SDL_SCANCODE_TAB]) SDL_PollEvent(&event);  //while space pressed do nothing and update event
            swapBoolStates(&gui->inventoryState);
        }

        if(keyboardState[SDL_SCANCODE_F3]) { //more info
            while(keyboardState[SDL_SCANCODE_F3]) SDL_PollEvent(&event);  //while F3 pressed do nothing and update event
            swapBoolStates(&debugMenu->active);
        }
        if(keyboardState[SDL_SCANCODE_F11]) {   // enable/disable fullscreen mode
            while (keyboardState[SDL_SCANCODE_F11]) SDL_PollEvent(&event);  //while F11 pressed do nothing and update event
            if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
                screen->h = 800;
                screen->w = 1400;
                SDL_SetWindowSize(window, screen->w, screen->h);
                SDL_SetWindowFullscreen(window, 0);
            } else {
                //set new window size
                if (SDL_GetDesktopDisplayMode(0, &screenSize) != 0) {   //get display size if error render will be in 720p 
                    cout << "Error fullscreen : " << SDL_GetError() << endl;
                    screen->h = 720;
                    screen->w = 1280;
                } else {
                    screen->h = screenSize.h;
                    screen->w = screenSize.w;
                }
                //change window size

                SDL_SetWindowSize(window, screen->w, screen->h);
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        }
        if(keyboardState[SDL_SCANCODE_ESCAPE] ) running = false;
        if(event.type == SDL_QUIT) running = false;
}

void GameManager::startMusic() {
    Mix_PlayMusic(ambiantMusic, -1);
}

void GameManager::stopMusic() {
    Mix_ResumeMusic();
}

void GameManager::setMusicVolume(int volume) {
    if(volume > 128) volume = 128;
    if(volume < 0) volume = 0;

    Mix_VolumeMusic(volume);
}
