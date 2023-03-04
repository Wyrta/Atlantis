#include "Printable.hpp"

#include "Console.hpp"

extern Console		*console;
extern SDL_Renderer *render;
extern SDL_Rect		screen;


bool Printable::debug = false;

SDL_Texture *createTexture(SDL_Rect *rectangle, const char *path)
{
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	surface = IMG_Load(path);

	if(surface) {
		texture = SDL_CreateTextureFromSurface(render, surface);
		SDL_FreeSurface(surface);

		if(rectangle != NULL)
			SDL_QueryTexture(texture, NULL, NULL, &rectangle->w, &rectangle->h);

		console->log("Create texture : \"%s\" at 0x%x", path, (unsigned int) texture);
		return (texture);
	}
	else {
		console->log(ERROR, "Create texture : \"%s\" : %s", path, SDL_GetError());
	}
	return (NULL);
}

Printable::Printable(const char *objname, const char *filepath)
{
	this->name.assign(objname);
	this->path.assign(filepath);

	this->texture = createTexture(&this->texture_rect, filepath);

	this->dst_rect.w = this->texture_rect.w;
	this->dst_rect.h = this->texture_rect.h;
}

Printable::~Printable()
{
	SDL_DestroyTexture(this->texture);
}

bool Printable::print()
{
	return (this->print(&this->src_rect, &this->dst_rect));
}

bool Printable::print(SDL_Rect *src, SDL_Rect *dst)
{
	int retval;

	if (this->texture == NULL)
	{
		console->log(ERROR, "Invalid texture (ptr : 0x%x) \"%s\"", (unsigned int) this->texture, this->name.c_str());
		return (false);
	}

	retval = SDL_RenderCopy(render, this->texture, src, dst);

	if (retval == 0)
	{
		return (true);
	}
	else
	{
		console->log(ERROR, "Cannot print \"%s\"", this->name.c_str());
		return (false);
	}
}


Entity::Entity(const char *entityName, const char *texturePath) : Printable(entityName, texturePath)
{
	this->position.x = 0;
	this->position.y = 0;

	dst_rect.x = this->position.x;
	dst_rect.y = this->position.y;
	dst_rect.w = dst_rect.h = 60;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = screen.w;
	src_rect.h = screen.h;
}


Entity::~Entity()
{
}


void Entity::move(Direction direction)
{
	switch (direction)
	{
		case NORTH: 
			this->position.y -= 10;
			break;
		case SOUTH: 
			this->position.y += 10;
			break;
		case WEST:  
			this->position.x -= 10;
			break;
		case EAST:  
			this->position.x += 10;
			break;
		
		default:
			break;
	}

	this->dst_rect.x = this->position.x;
	this->dst_rect.y = this->position.y;
}

bool Entity::print_onMap(SDL_Point offset)
{
	SDL_Rect offsetRect;
	offsetRect.x = offset.x + this->position.x;
	offsetRect.y = offset.y + this->position.y;
	offsetRect.w = dst_rect.w;
	offsetRect.h = dst_rect.h;

	return (this->print(&this->src_rect, &offsetRect));
}