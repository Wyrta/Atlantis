#ifndef _printable_hpp_
#define _printable_hpp_

#include "../include/SDL/SDL.h"
#include "../include/SDL_ttf/SDL_ttf.h"
#include "../include/SDL_image/SDL_image.h"

#include <string>
#include <vector>
using namespace std;

SDL_Texture *createTexture(SDL_Rect *rectangle, const char *path);
SDL_Texture *write(SDL_Rect *rect, TTF_Font *font, const char *text, SDL_Color color);
TTF_Font	*createFont(const char *path, int size);

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


typedef enum {
	AVARA,
	DUNO,
	LAST_FONT		/* Dont use */
} Font_type;

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
		SDL_Rect	texture_rect;

	protected:
		std::string name = "";
		std::string path = "";

		SDL_Rect src_rect;
		SDL_Rect dst_rect;

		static vector<Printable *>toDebug;
		bool destroy_texture = true;

	public:
		Printable(SDL_Rect size);
		Printable(const char *objname, const char *filepath);
		~Printable();

		static bool debug;
		static void proc_debug(void);

		bool		print();
		bool		print(SDL_Rect *src_rect, SDL_Rect *dst_rect);
		virtual void print_debug(void);

		void		setTexture(const char *filepath);
		void		setTexture(SDL_Texture *ptr_texture);
		SDL_Rect	getHitbox() { return this->texture_rect; }
		void		setHitbox() { SDL_QueryTexture(this->texture, NULL, NULL, &this->texture_rect.w, &this->texture_rect.h); }

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

		bool		print_onMap(SDL_Point offset);
		void		print_debug(void);

		void		setPosition(int x, int y);
		SDL_Point	getPosition(void) { return (this->position); }
		Direction	getWalkable() 	{ return (this->walkable); }

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
		void		print_debug(void);

		void		proc(void);
		void		move(Direction direction, Tile *tile);

		SDL_Point	getPosition(void)			{ return this->position; }
		SDL_Point	getPosition_screen(void)	{ return this->positionScreen; }
		Direction	isMoving(void)				{ return this->moving; }
		bool		canMove(void)				{ return (this->moving == Direction::NONE); }
};


class Text : public Printable
{
	private:
		string		text;
		Font_type	type;
		SDL_Point	position;

		static TTF_Font *fonts[Font_type::LAST_FONT];

	public:
		Text(const char *content, Font_type fontype, SDL_Point pos = {0, 0});
		~Text();
		
		bool		print_onMap(SDL_Point offset = {0, 0});

		string 		getText() { return (text); }

		static void load_font(void);
		static void unload_font(void);
};



#endif /* _printable_hpp_ */
