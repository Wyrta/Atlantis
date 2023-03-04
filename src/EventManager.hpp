#ifndef _eventmanager_hpp_
#define _eventmanager_hpp_

#include "../include/SDL/SDL.h"
#include "Statemachine.hpp"

#define MAX_EVENT_POLL      100

class EventManager
{
    private:
	    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        State *appState;

    public:
        EventManager(State *);
        ~EventManager();

        void pollEvent(void);
        const Uint8 *getKeyboard(void);
        bool getKey(SDL_Scancode );

        bool eventLog;
        SDL_Event event;

};



#endif /* _eventmanager_hpp_ */
