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


Map::Map(string mapname)
{
	this->name = mapname;

	this->load(mapname.c_str());


}


Map::~Map()
{
	this->reset();

}


void Map::print(void)
{
	for (unsigned int i = 0; i < tilemap.size(); i++)
	{
		tilemap[i]->print_onMap(position);
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
		tilePos = tilemap[i]->getPosition();
		
		if ((pos.x == tilePos.x) && (pos.y == tilePos.y))
		{
			tileFound[tileFoundCpt++] = tilemap[i];
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


void Map::load(const char *filename)
{
	char filepath[256];
	sprintf(filepath, "maps/%s.map", filename);
	console->log("Loading map %s ...", filepath);

	ifstream	config(filepath);
	int 		nb_tiles = 0;
	int 		nb_lines = 0;

	string		line;
	int			l_idx, r_idx;
	Tile_params params;

	this->reset();

	while (getline(config, line) )
	{
		l_idx = 0;
		r_idx = line.find(";");

		nb_lines++;

		while (r_idx != (int)string::npos)
		{
			string strTile, strTile_x, strTile_y, strTile_type, strTile_walkable;
			int comma_idx[3];
			int same_pos_tile = 0;

			if (l_idx > r_idx)
			{
				console->log(log_t::ERROR, "\tInvalid index l_idx > r_idx (%s > %s)", l_idx, r_idx);
				return;
			}

			strTile				= line.substr(l_idx, r_idx - l_idx);

//			console->log("\"%s\"", strTile.c_str());

			comma_idx[0]		= strTile.find(',', 0);
			comma_idx[1]		= strTile.find(',', comma_idx[0]+1);
			comma_idx[2]		= strTile.find(',', comma_idx[1]+1);

			strTile_x 			= strTile.substr(0	           , comma_idx[0]);
			strTile_y 			= strTile.substr(comma_idx[0]+1, comma_idx[1] - comma_idx[0]-1);
			strTile_type		= strTile.substr(comma_idx[1]+1, comma_idx[2] - comma_idx[1]-1);
			strTile_walkable	= strTile.substr(comma_idx[2]+1, r_idx        - comma_idx[2]-1);

			params.x			= atoi(strTile_x.c_str());
			params.y			= atoi(strTile_y.c_str());
			params.type			= Tile::getTiletype(strTile_type);
			params.walkable		= Tile::getTileDir(strTile_walkable);

//			console->log("> %d,%d,%d,%d", params.x, params.y, params.type, params.walkable);

			for (unsigned int i = 0; i < tilemap.size(); i++)
			{
				if ((tilemap[i]->getPosition().x == params.x) && (tilemap[i]->getPosition().y == params.y))
					same_pos_tile++;
			}

			if (same_pos_tile > 0)
				console->log(log_t::WARNING, "Already have %d tile%s on this position (x:%d y:%d)", same_pos_tile, (same_pos_tile == 1) ? "" : "s", params.x, params.y);

			tilemap.push_back(new Tile(params) );

			nb_tiles++;

			l_idx = r_idx +1;
			r_idx = line.find(";", l_idx);
		}
	}

	console->log("Succesfully load map %s (%d Tiles in %d Lines)", filename, nb_tiles, nb_lines);

	config.close();
}

void Map::reset(void)
{
	for (unsigned int i = 0; i < tilemap.size(); i++)
	{
		delete tilemap[i];
	}

	this->tilemap.clear();

}