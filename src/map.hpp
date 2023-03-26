#ifndef _map_hpp_
#define _map_hpp_

#include <vector>

using namespace std;

#include "Printable.hpp"
#include "../include/SDL/SDL.h"

class Map {
    public:
        Map(string mapname);
        ~Map();

        void		print(void);

		void		focus(SDL_Point pos);
		
		SDL_Point	getPosition(void) { return (this->position); }
		Tile		*getTile(SDL_Point pos);
    private:
		string		name;
		SDL_Point	position;
		
		vector<Tile *> tilemap;

		void load(const char *filename);
		void reset(void);
};

#endif /* _map_hpp_ */