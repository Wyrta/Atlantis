#include "map.hpp"
#include "console.hpp"

extern SDL_Renderer *render;
extern SDL_Window   *window;
extern Console	  *console; 


string map(SDL_Rect *screen, string mapName) {
	int fade = 255;

	/******************Create gameManager, GUI and debug**********************/

	MapManager mMap(mapName);

	GameManager gameManager("Orchestre_Wild_Earth");
		gameManager.startMusic();
		gameManager.set_fps(60);


	GUI gui(*screen);
	DebugMenu debugMenu("./Avara.ttf");

	SDL_Texture *bgtexture = createTexture(render, NULL, "./img/background_menu.png");


	/****************************Create entity********************************/

	Player player(mMap.initPos(), "player", screen, 0);

	Entity entity1(mMap.initPos(), "bot1");
		entity1.setSpeed(4);

	Npc npc1((SDL_Point){1500, 500}, "npcTest", 0);
	Npc npc2((SDL_Point){1700, 500}, "npcTest", 10);
	Npc npc3((SDL_Point){1900, 500}, "npcTest", 3);
	

	/***************************some little var*******************************/

	gameManager.eventHandler(window, screen, &gui, &debugMenu, &mMap, &player);

	gameManager.targetPosition.x = player.getPosition().x;
	gameManager.targetPosition.y = player.getPosition().y;

	mMap.rectMap.x = (-player.getPosition().x) + screen->w/2;
	mMap.rectMap.y = (-player.getPosition().y) + screen->h/2;

	/*******************************game loop***********************************/
	while(gameManager.running) {

		/***************************event management*******************************/

		/* Event manager */
		gameManager.eventHandler(window, screen, &gui, &debugMenu, &mMap, &player);

		/* entity managing */
		player.follow(gameManager.targetPosition, mMap.rectMap, *screen, mMap.hitbox);
		npc1.idleTalk();
		entity1.follow(player.getPosition(), mMap.rectMap, *screen, mMap.hitbox);


		/***************************Start render*******************************/

		/* clear previous render screen */
		SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
		SDL_RenderFillRect(render, screen);			

		/* Map */
		SDL_RenderCopy(render, bgtexture, NULL, NULL);
		mMap.print(debugMenu.active);

		/* Entity */
		for(int i = 0; i < MAX_ENTITY; i++)
		{
			if (Entity::entityTab[i] != NULL)
				Entity::entityTab[i]->print(mMap.rectMap);
		}	
		entityProcAll();
		//entityRenderAll(&mMap);

		/* GUI */
		gui.followPlayer(*screen, gameManager.screenFollowPlayer); /* button  */
		if(gameManager.screenFollowPlayer) 
			player.centerCamera(&mMap.rectMap, *screen);

		gui.inventory(*screen, &player, gui.inventoryState);
		debugMenu.print(gameManager.get_renderTime()); /* display debug menu */
		console->print();

		/* fade in */
		if(fade > 0)
		{
			fade = fade - 3;
			SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(render, 0, 0, 0, fade);
			SDL_RenderFillRect(render, screen);	  
		}


		/* Render */
		SDL_RenderPresent(render);

		/***************************loop delay*******************************/

		gameManager.fpsLimiter();
	}

	gameManager.stopMusic();

	SDL_DestroyTexture(bgtexture);

	return mMap.nextMapName;
}
