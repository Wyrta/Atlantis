#ifndef _statemachine_hpp_
#define _statemachine_hpp_

typedef enum {
    INIT,
    MENU,
    GAME,
    EXIT
} State;

int init(State *);
int menu(State *);
int game(State *);
int exit(State *);


#endif /* _statemachine_hpp_ */
