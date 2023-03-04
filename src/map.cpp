#include "Map.hpp"

#include "Console.hpp"

extern Console *console;
extern SDL_Rect screen;

Map::Map(const char *filepath)
{
	for (int i = 0; i < NB_MAP_LVL; i++)
		maplvl[i] = NULL;

	maplvl[1] = new Printable("Map test", filepath);

	mapHitbox = maplvl[1]->getHitbox();
	mapHitbox.x = mapHitbox.y = 0;
//	mapHitbox.w *= 2;
//	mapHitbox.h *= 2;
}


Map::~Map()
{
	for (int i = 0; i < NB_MAP_LVL; i++)
	{
		if (maplvl[i] != NULL) {
			delete (maplvl[i]);
			maplvl[i] = NULL; 
		}
	}
}


void Map::print(void)
{
	mapHitbox.x = this->position.x;
	mapHitbox.y = this->position.y;

	for (int i = 0; i < NB_MAP_LVL; i++)
	{
		if (maplvl[i] != NULL)
			maplvl[i]->print(NULL, &mapHitbox);
	}
}

void Map::focus(SDL_Point pos)
{
	this->position.x = (-pos.x) + (screen.w/2);
	this->position.y = (-pos.y) + (screen.h/2);

}

