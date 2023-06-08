#ifndef _statemachine_hpp_
#define _statemachine_hpp_

#include "Printable.hpp"

typedef enum {
    INIT,
    MENU,
    GAME,
	FIGHT,
    EXIT
} State;

extern Entity *fighter_top;
extern Entity *fighter_bot;

int init(State *);
int menu(State *);
int game(State *);
int fight(State *);
int exit(State *);


#endif /* _statemachine_hpp_ */
