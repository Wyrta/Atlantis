#ifndef _map_hpp_
#define _map_hpp_

#include <vector>

using namespace std;

#include "Printable.hpp"
#include "../include/SDL/SDL.h"
#define NB_MAP_LVL      3

typedef enum {
	TEST,
	TEST2,
	LAST_MLVL		/* Dont use */
} Map_lvl;

class Map {
    public:
        Map(Map_lvl name);
        ~Map();

        void print(void);

		void focus(SDL_Point pos);
		SDL_Point getPosition(void) { return (this->position); }
    private:

		vector<Tile> tilemap;

		SDL_Point	position;
};

#endif /* _map_hpp_ */