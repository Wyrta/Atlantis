#ifndef _eventmanager_hpp_
#define _eventmanager_hpp_

#include "../include/SDL/SDL.h"
#include "Statemachine.hpp"

#define MAX_EVENT_POLL      100

#define getKeyK(x)	getKeySC(SDL_GetScancodeFromKey(x))


typedef struct Mouse_s
{
	int x, y;

	bool left, right;

	bool click_left, click_right;

	bool last_left, last_right;	/* dont use */

/* > 0 UP 
< 0 DOWN */
	int wheel;	
} Mouse_t;

class EventManager
{
    private:
	    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        Uint8 last_keyboardState[SDL_Scancode::SDL_NUM_SCANCODES];
        State *appState;

    public:
        EventManager(State *);
        ~EventManager();

        void        pollEvent(void);
        const Uint8 *getKeyboard(void);
        bool        getKeySC(SDL_Scancode );
        bool        getKeyUp(SDL_Scancode );
        bool        getKeyDown(SDL_Scancode );

        bool        eventLog;
        SDL_Event   event;
};



#endif /* _eventmanager_hpp_ */
