#include "map_test.hpp"

bool map_test(SDL_Renderer *render, SDL_Window *window, SDL_Rect screen) {
    bool running = true;

    SDL_Surface *surface = IMG_Load("./img/background_menu.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
    if(texture == NULL) {cout << "error on texture : " << SDL_GetError() << endl; SDL_Delay(5000); return 1;}
    SDL_FreeSurface(surface);


    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        SDL_Scancode Z_key = SDL_GetScancodeFromKey(SDLK_z);
        SDL_Scancode S_key = SDL_GetScancodeFromKey(SDLK_s);
        SDL_Scancode Q_key = SDL_GetScancodeFromKey(SDLK_q);
        SDL_Scancode D_key = SDL_GetScancodeFromKey(SDLK_d);

        SDL_Scancode up_key    = SDL_GetScancodeFromKey(SDLK_UP);
        SDL_Scancode down_key  = SDL_GetScancodeFromKey(SDLK_DOWN);
        SDL_Scancode left_key  = SDL_GetScancodeFromKey(SDLK_LEFT);
        SDL_Scancode right_key = SDL_GetScancodeFromKey(SDLK_RIGHT);


    //***********************Create player texture ****************************

    SDL_Texture *texturePlayer = NULL;
    SDL_Rect player;
    texturePlayer = createTexture(render, &player, "./img/entity/persoTest.png");
    player.w = 100; player.h = 100;
    player.x = screen.w/2 - player.w/2;
    player.y = screen.h/2 - player.h/2;

    SDL_Point playerCoo;
        playerCoo.x = screen.w/2;
        playerCoo.y = screen.h/2;


    //***********************Create map texture ****************************
    
    SDL_Texture *textureMap = NULL;
    SDL_Rect rectMap;
    textureMap = createTexture(render, &rectMap, "./img/map/plage.png");
    rectMap.x = 0;
    rectMap.y = 0;


    //***********************Create debug texture ****************************
    
    TTF_Font *debugFont = createFont("./Avara.ttf", 16);

    SDL_Texture *fpsDisplay = NULL;
    SDL_Rect rectFpsDisplay;
    SDL_Texture *fpsDisplay2 = NULL;
    SDL_Rect rectFpsDisplay2;

    SDL_Texture *texturePosPLayer = NULL;
    SDL_Rect rectTexturePosPLayer;
    SDL_Texture *texturePosPLayerX = NULL;
    SDL_Rect rectTexturePosPLayerX;
    SDL_Texture *texturePosPLayerY = NULL;
    SDL_Rect rectTexturePosPLayerY;

    fpsDisplay  = write(render, &rectFpsDisplay, debugFont, "FPS:");
        rectFpsDisplay.x = 0; rectFpsDisplay.y = 0;  
    fpsDisplay2 = write(render, &rectFpsDisplay2, debugFont, "358105");
        rectFpsDisplay2.x = rectFpsDisplay.w + 3;
        rectFpsDisplay2.y = 0;

    texturePosPLayer = write(render, &rectTexturePosPLayer, debugFont, "Position (X, Y) :");
        rectTexturePosPLayer.x = 0;
        rectTexturePosPLayer.y = rectFpsDisplay.h + 5;
    texturePosPLayerX = write(render, &rectTexturePosPLayerX, debugFont, "_");
        rectTexturePosPLayerX.x = 0;
        rectTexturePosPLayerX.y = rectTexturePosPLayer.x + rectTexturePosPLayer.w;
    texturePosPLayerY = write(render, &rectTexturePosPLayerY, debugFont, "_");
        rectTexturePosPLayerY.x = 0;
        rectTexturePosPLayerY.y = rectTexturePosPLayerX.x + rectTexturePosPLayerX.w;
    
    //***********************some little var****************************
    SDL_Event event;
    SDL_Point mouse; Uint32 buttons;
    buttons = SDL_GetMouseState(&mouse.x, &mouse.y);

    SDL_Point targetPosition;
        targetPosition.x = playerCoo.x;
        targetPosition.y = playerCoo.y;

    bool debug = false;
    while(running) {
/*
        while(SDL_PollEvent(&event)) {  // poll until all events are handled!
            //If keypressed
            if(event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_z: {player.y -= 10;} break;
                    case SDLK_s: {player.y += 10;} break;
                    case SDLK_q: {player.x -= 10;} break;
                    case SDLK_d: {player.x += 10;} break;
                    case SDLK_ESCAPE: running = false;
                default: break;
                }
            }
            //If user want to quit
            if(event.type == SDL_QUIT) running = false;
        }
*/
        
        SDL_PollEvent(&event);
            buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
            if ((buttons & SDL_BUTTON_RMASK) != 0) {
                int x = 0;
                int y = 0;
                SDL_GetWindowPosition(window, &x, &y);
                targetPosition.x = mouse.x - rectMap.x;
                targetPosition.y = mouse.y - rectMap.y;

                if(targetPosition.x < 0) targetPosition.x = 0;
                if(targetPosition.y < 0) targetPosition.y = 0;
                if(targetPosition.x > rectMap.w) targetPosition.x = rectMap.w;
                if(targetPosition.y > rectMap.h) targetPosition.y = rectMap.h;
            }
/* movement with zqsd
            if(keyboardState[Z_key] != 0 && playerCoo.y > 0) playerCoo.y -= 10;
            if(keyboardState[S_key] != 0 && playerCoo.y + player.h < rectMap.h) playerCoo.y += 10;
            if(keyboardState[Q_key] != 0 && playerCoo.x > 0) playerCoo.x -= 10;
            if(keyboardState[D_key] != 0 && playerCoo.x + player.w < rectMap.w) playerCoo.x += 10;
*/
            if(keyboardState[up_key   ] != 0 ) rectMap.y += 10;
            if(keyboardState[down_key ] != 0 ) rectMap.y -= 10;
            if(keyboardState[left_key ] != 0 ) rectMap.x += 10;
            if(keyboardState[right_key] != 0 ) rectMap.x -= 10;

            if(keyboardState[SDL_SCANCODE_F3]) {
                while (keyboardState[SDL_SCANCODE_F3]) SDL_PollEvent(&event);  //while F3 pressed do nothing and update event
                swapBoolStates(&debug);
                }
            if(keyboardState[SDL_SCANCODE_F11]) {   // enable/disable fullscreen mode
                while (keyboardState[SDL_SCANCODE_F11]) SDL_PollEvent(&event);  //while F11 pressed do nothing and update event
                if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
                    screen.h = 800;
                    screen.w = 1400;
                    SDL_SetWindowSize(window, screen.w, screen.h);
                    SDL_SetWindowFullscreen(window, 0);

                    cout << "windowed screen size : " << endl << screen.h << endl << screen.w << endl;
                } else {
                    //set new window size

                    SDL_DisplayMode dm;
                    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {   //get display size if error render will be in 720p 
                        cout << "Error fullscreen : " << SDL_GetError() << endl;
                        screen.h = 720;
                        screen.w = 1280;
                    } else {
                        screen.h = dm.h;
                        screen.w = dm.w;
                    }
                    //change window size

                    SDL_SetWindowSize(window, screen.w, screen.h);
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    cout << "fullscreen size : " << endl << screen.h << endl << screen.w << endl;
                }
            }
            if(keyboardState[SDL_SCANCODE_ESCAPE] ) running = false;
            if(event.type == SDL_QUIT) running = false;

        //rectMap.x = -(playerCoo.x - (screen.w/2 - player.w/2) );
        //rectMap.y = -(playerCoo.y - (screen.h/2 - player.h/2) );


        //cout << "Player \nX : " << playerCoo.x << " map w : " << rectMap.w << endl << "Y : " << playerCoo.y << " map h : " << rectMap.h << endl;

        if(targetPosition.x > playerCoo.x + 10) playerCoo.x += 5;
        if(targetPosition.x < playerCoo.x - 10) playerCoo.x -= 5;

        if(targetPosition.y > playerCoo.y + 10) playerCoo.y += 5;
        if(targetPosition.y < playerCoo.y - 10) playerCoo.y -= 5;

        player.x = playerCoo.x + rectMap.x - player.w/2;
        player.y = playerCoo.y + rectMap.y - player.h/2;

        //Start render
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);   //clear previous render screen
        SDL_RenderFillRect(render, &screen);            //

        SDL_RenderCopy(render, texture, NULL, NULL);
        SDL_RenderCopy(render, textureMap, NULL, &rectMap);
        SDL_RenderCopy(render, texturePlayer, NULL, &player);
        if(debug) { //display debug menu
            //display fps
            SDL_RenderCopy(render, fpsDisplay, NULL, &rectFpsDisplay);
            SDL_RenderCopy(render, fpsDisplay2, NULL, &rectFpsDisplay2);

            //update player position
            SDL_DestroyTexture(texturePosPLayerX);
            SDL_DestroyTexture(texturePosPLayerY);

            texturePosPLayerX = write(render, &rectTexturePosPLayerX, debugFont, to_string(playerCoo.x).c_str());
                rectTexturePosPLayerX.x = rectTexturePosPLayer.y;
                rectTexturePosPLayerX.y = rectTexturePosPLayer.y + rectTexturePosPLayer.h;
            texturePosPLayerY = write(render, &rectTexturePosPLayerY, debugFont, to_string(playerCoo.y).c_str());
                rectTexturePosPLayerY.x = rectTexturePosPLayer.y;
                rectTexturePosPLayerY.y = rectTexturePosPLayerX.y + rectTexturePosPLayerX.h;

            //display player position
            SDL_RenderCopy(render, texturePosPLayer, NULL, &rectTexturePosPLayer);
            SDL_RenderCopy(render, texturePosPLayerX, NULL, &rectTexturePosPLayerX);
            SDL_RenderCopy(render, texturePosPLayerY, NULL, &rectTexturePosPLayerY);
        }
        SDL_RenderPresent(render);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(textureMap);

    return false;
}