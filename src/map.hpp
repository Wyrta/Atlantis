#ifndef _map_hpp_
#define _map_hpp_

#include "Printable.hpp"
#include "../include/SDL/SDL.h"

#define NB_MAP_LVL      3

class Map {
    public:
        Map(const char *filepath);
        ~Map();

        void print(void);
    private:
        Printable *maplvl[NB_MAP_LVL];   /* 0 hitbox, 1 ground, ... */
        SDL_Rect   mapHitbox;
};

#endif /* _map_hpp_ */