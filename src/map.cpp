#include "map.hpp"





string map(SDL_Renderer *render, SDL_Window *window, SDL_Rect screen, string mapName) {

    /******************Create gameManager, GUI and debug**********************/
    
    MapManager mMap(render, mapName);

    GameManager gameManager("Bossa_Novac");
        gameManager.startMusic();

    GUI gui(render, screen);
    DebugMenu debugMenu(render);

    
    SDL_Texture *bgtexture = createTexture(render, NULL, "./img/background_menu.png");



    /****************************Create entity********************************/

    Player player(render, mMap.initPos(), "persoTest");

    Entity entity1(render, mMap.initPos(), "persoTest");
        entity1.speed = 4;

    //why not a tab of entity here


    /***************************some little var*******************************/

    gameManager.eventHandler(window, &screen, &gui, &debugMenu, &mMap, &player);

    gameManager.targetPosition.x = player.mPosition.x;
    gameManager.targetPosition.y = player.mPosition.y;

    mMap.rectMap.x = (-player.mPosition.x) + screen.w/2;
    mMap.rectMap.y = (-player.mPosition.y) + screen.h/2;



    while(gameManager.running) {
        //Event manager
        gameManager.eventHandler(window, &screen, &gui, &debugMenu, &mMap, &player);

        //entity managing
        player.follow(gameManager.targetPosition, mMap.rectMap, screen, mMap.hitbox);
        entity1.follow(player.mPosition, mMap.rectMap, screen, mMap.hitbox);

        //Start render
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);   //clear previous render screen
        SDL_RenderFillRect(render, &screen);            //

        //Map
        SDL_RenderCopy(render, bgtexture, NULL, NULL);
        mMap.print(render, debugMenu.active);

        //Entity
        player.print(render, debugMenu.active);
        entity1.print(render, debugMenu.active);

        //GUI
        gui.followPlayer(render, screen, gameManager.screenFollowPlayer); //button 
        if(gameManager.screenFollowPlayer) 
            player.centerCamera(&mMap.rectMap, screen);
        gui.inventory(render, screen, &player, gui.inventoryState);
        if(debugMenu.active) //display debug menu
            debugMenu.print(render, 144, player.mPosition);

        //Render
        SDL_RenderPresent(render);
        SDL_Delay(16);//**********************************************************************************
    }

    gameManager.stopMusic();

    SDL_DestroyTexture(bgtexture);

    return mMap.nextMapName;
}