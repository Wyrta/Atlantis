#ifndef _printable_hpp_
#define _printable_hpp_

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"

#include <string>

SDL_Texture *createTexture(SDL_Rect *rectangle, const char *path);

#define TILESIZE		64
#define ENTITYSPEED		4


typedef enum {
	NORTH,
	SOUTH,
	WEST,
	EAST,
	ALL,
	NONE
} Direction;

typedef enum {
	DIRT,
	DIRT_PATH_1,
	DIRT_PATH_2,
	DIRT_PATH_3,
	DIRT_PATH_4,
	DIRT_NE_GRASS,
	DIRT_NW_GRASS,
	DIRT_SE_GRASS,
	DIRT_SW_GRASS,
	GRASS_E_W_DIRT,
	GRASS_N_S_DIRT,
	GRASS_NE_DIRT,
	GRASS_NW_DIRT,
	GRASS_S_N_DIRT,
	GRASS_SE_DIRT,
	GRASS_SW_DIRT,
	GRASS_W_E_DIRT,
	GRASS_1,
	GRASS_2,
	GRASS_3,
	TINYBUSH_1,
	TINYBUSH_2,
	TINYBUSH_3,
	TINYBUSH_4,
	EMPTY,
	LAST_TTYPE		/* Dont use */
} Tile_type;


typedef struct {
	int x;
	int y;

	Tile_type type;
	
	Direction walkable;
} Tile_params;


class Printable
{
	private:
		SDL_Texture *texture = NULL;

		SDL_Rect texture_rect;

		bool destroy_texture = true;
	protected:
		std::string name = "";
		std::string path = "";

		SDL_Rect src_rect;
		SDL_Rect dst_rect;

	public:
		Printable(SDL_Rect size);
		Printable(const char *objname, const char *filepath);
		~Printable();

		bool print();
		bool print(SDL_Rect *src_rect, SDL_Rect *dst_rect);


		void setTexture(const char *filepath);
		void setTexture(SDL_Texture *ptr_texture);

		SDL_Rect getHitbox() { return texture_rect; }

		static bool debug;
};


class Tile : public Printable
{
	private:
		static SDL_Texture *texture[Tile_type::LAST_TTYPE];

		Tile_type type;
		Direction walkable;

		SDL_Point position;

	public:
	
		Tile(Tile_params params);
		Tile(Tile_type tiletype = EMPTY, SDL_Rect info = {0, 0, TILESIZE, TILESIZE});
		~Tile();

		bool print_onMap(SDL_Point offset);
		void setPosition(int x, int y);
		SDL_Point getPosition(void) { return (this->position); }
		Direction getWalkable() 	{ return (this->walkable); }


		static void load_all_texture();
		static void unload_all_texture();
};


class Entity : public Printable
{
	private:
		SDL_Point	position;
		SDL_Point 	positionScreen;
		Direction	moving;


	public:
		Entity(const char *entityName, const char *texturePath);
		~Entity();

		bool		print_onMap(SDL_Point offset);

		void		proc(void);
		void		move(Direction direction, Tile *tile);

		SDL_Point	getPosition(void)			{ return this->position; }
		SDL_Point	getPosition_screen(void)	{ return this->positionScreen; }
		Direction	isMoving(void)				{ return this->moving; }
		bool		canMove(void)				{ return (this->moving == Direction::NONE); }
};



#endif /* _printable_hpp_ */
