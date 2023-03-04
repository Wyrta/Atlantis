#ifndef _printable_hpp_
#define _printable_hpp_

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"

#include <string>

SDL_Texture *createTexture(SDL_Rect *rectangle, const char *path);

class Printable
{
	private:
		SDL_Texture *texture = NULL;

		SDL_Rect texture_rect;


	protected:
		std::string name = "";
		std::string path = "";

		SDL_Rect src_rect;
		SDL_Rect dst_rect;

	public:
		Printable(const char *objname, const char *filepath);
		~Printable();

		bool print();
		bool print(SDL_Rect *src_rect, SDL_Rect *dst_rect);

		SDL_Rect getHitbox() { return texture_rect; }

		static bool debug;
};

typedef enum {
	NORTH,
	SOUTH,
	WEST,
	EAST
} Direction;

class Entity : public Printable
{
	private:
		SDL_Point position;
	public:
		Entity(const char *entityName, const char *texturePath);
		~Entity();

		bool print_onMap(SDL_Point offset);

		void move(Direction direction);
		SDL_Point getPosition() { return this->position; }

};



#endif /* _printable_hpp_ */
