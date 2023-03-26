#include "Map.hpp"

#include <fstream>
#include "Console.hpp"

using namespace std;

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
			load_map((char *)"test");
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


void Map::load_map(char *filename)
{
	char filepath[256];
	sprintf(filepath, "maps/%s.map", filename);
	console->log("Loading map %s ...", filepath);

	ifstream	config(filepath);
	string		line;
	int			l_idx, r_idx;
	int 		nb_tiles = 0;
	int 		tile_x, tile_y;


	this->tilemap.clear();

	while (getline(config, line) )
	{
		console->log("Detect new line %s", line.c_str());

		l_idx = 0;
		r_idx = line.find(";");

		while (r_idx != (int)string::npos)
		{
			string strTile, strTile_x, strTile_y, strTile_type, strTile_walkable;
			int comma_idx[3];
			
			if (l_idx > r_idx)
			{
				console->log(log_t::ERROR, "Invalid index l_idx > r_idx (%s > %s)", l_idx, r_idx);
				return;
			}

			strTile				= line.substr(l_idx, r_idx);

			console->log("\t\"%s\"", strTile.c_str());

			comma_idx[0]		= strTile.find(',', l_idx+1);
			comma_idx[1]		= strTile.find(',', comma_idx[0]+1);
			comma_idx[2]		= strTile.find(',', comma_idx[1]+1);

			strTile_x 			= strTile.substr(l_idx+1	   , comma_idx[0] - l_idx);
			strTile_y 			= strTile.substr(comma_idx[0]+1, comma_idx[1] - comma_idx[0]-1);
			strTile_type		= strTile.substr(comma_idx[1]+1, comma_idx[2] - comma_idx[1]-1);
			strTile_walkable	= strTile.substr(comma_idx[2]+1, r_idx        - comma_idx[2]-1);

			tile_x				= atoi(strTile_x.c_str());
			tile_y				= atoi(strTile_y.c_str());

			console->log("\t> %d,%d,%s,%s", tile_x, tile_y, strTile_type.c_str(), strTile_walkable.c_str());

			l_idx = r_idx;
			r_idx = line.find(";", l_idx);
			console->log("l_idx: %d, r_idx: %d", l_idx, r_idx);

		}
	}

	console->log("Succesfully load map %s (%d Tiles)", nb_tiles);

	config.close();
}

