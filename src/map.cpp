#include "Map.hpp"

#include "Console.hpp"

extern Console *console;
extern SDL_Rect screen;

Tile_params maps_TEST[] = {
#include "maps/test.rc"
};

Tile_params maps_TEST2[] = {
#include "maps/test2.rc"
};


Map::Map(Map_lvl mapname)
{
	this->name = mapname;

	switch (this->name)
	{
		case Map_lvl::TEST:		
			for (int i = 0; i < (int)(sizeof(maps_TEST)/sizeof(Tile_params)); i++)
			{
				tilemap.push_back(Tile(maps_TEST[i]) );
			}
		break;
		case Map_lvl::TEST2:	
			for (int i = 0; i < (int)(sizeof(maps_TEST2)/sizeof(Tile_params)); i++)
			{
				tilemap.push_back(Tile(maps_TEST[i]));
			}
		break;

		default:
			break;
	}

}


Map::~Map()
{

}


void Map::print(void)
{
	for (unsigned int i = 0; i < tilemap.size(); i++)
	{
		tilemap[i].print_onMap(position);
	}
}


void Map::focus(SDL_Point pos)
{
	this->position.x = (-pos.x) + (screen.w/2);
	this->position.y = (-pos.y) + (screen.h/2);

}


Tile *Map::getTile(SDL_Point pos)
{
	Tile *tileFound[] = {NULL, NULL, NULL, NULL};
	int tileFoundCpt = 0;
	SDL_Point tilePos;
	
	for (unsigned int i = 0; i < tilemap.size(); i++)
	{
		tilePos = tilemap[i].getPosition();
		
		if ((pos.x == tilePos.x) && (pos.y == tilePos.y))
		{
			tileFound[tileFoundCpt++] = &tilemap[i];
		}
	}

	if (tileFoundCpt > 0)
	{
		if (tileFoundCpt == 1)
		{

			return (tileFound[0]);
		}
		else
		{
			console->log("Muliple tile found (%d) : returned last found", tileFoundCpt);
			
			return (tileFound[tileFoundCpt-1]);
		}
	}
	
	return (NULL);
}
