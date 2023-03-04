#include "Map.hpp"

#include "Console.hpp"

extern Console *console;
extern SDL_Rect screen;

Tile_params maps_TEST[] = {
#include "maps/test.rc"
};


Map::Map(Map_lvl name)
{
	switch (name)
	{
		case Map_lvl::TEST:		
			for (int i = 0; i < (int)(sizeof(maps_TEST)/sizeof(Tile_params)); i++)
			{
				tilemap.push_back(Tile(maps_TEST[i]) );
			}
		break;
		case Map_lvl::TEST2:	
			for (int i = 0; i < (int)(sizeof(maps_TEST)/sizeof(Tile_params)); i++)
			{
				tilemap.push_back(Tile(DIRT, {i, 0, TILESIZE, TILESIZE}) );
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

