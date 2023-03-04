#include "Printable.hpp"
#include <fstream>

#include "Console.hpp"
#include "EventManager.hpp"

extern Console		*console;
extern SDL_Renderer *render;
extern SDL_Rect		screen;
extern Mouse_t		mouse;


bool		Printable::debug = false;
SDL_Texture *Tile::texture[Tile_type::LAST_TTYPE];


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


Printable::Printable(SDL_Rect size)
{
	char printableName[32]; 
	sprintf(printableName, "[Printable] 0x%x", (unsigned int)this);

	this->name.assign(printableName);
	this->path.assign("");

	this->texture = NULL;

	this->destroy_texture = false;

	this->dst_rect.w = size.w;
	this->dst_rect.h = size.h;
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
	if (this->destroy_texture)
		SDL_DestroyTexture(this->texture);
	else
		this->texture = NULL;
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


void Printable::setTexture(const char *filepath)
{
	console->log("void Printable::setTexture(const char *filepath) > marche pas encore");
}


void Printable::setTexture(SDL_Texture *ptr_texture)
{
	if (this->texture != NULL)
	{
		SDL_DestroyTexture(this->texture);
		this->texture = NULL;	
	}
	
	this->texture = ptr_texture;
}


Tile::Tile(Tile_params params) : Printable({params.x, params.y, TILESIZE, TILESIZE})
{
	this->position.x = params.x;
	this->position.y = params.y;

	this->walkable = params.walkable;

	switch (params.type)
	{
	
		case DIRT:	this->setTexture(Tile::texture[DIRT]);	break;
		case WATER:	this->setTexture(Tile::texture[WATER]);	break;
		case STONE:	this->setTexture(Tile::texture[STONE]);	break;
		case BUSH:	this->setTexture(Tile::texture[BUSH]);	break;
		case EMPTY: this->setTexture(Tile::texture[DIRT]);	break;
		default:
			break;
	}
}


Tile::Tile(Tile_type type, SDL_Rect info) : Printable(info)
{
	this->position.x = info.x;
	this->position.y = info.y;

	switch (type)
	{
	
		case DIRT:	this->setTexture(Tile::texture[DIRT]);	break;
		case WATER:	this->setTexture(Tile::texture[WATER]);	break;
		case STONE:	this->setTexture(Tile::texture[STONE]);	break;
		case BUSH:	this->setTexture(Tile::texture[BUSH]);	break;
		case EMPTY: this->setTexture(Tile::texture[DIRT]);	break;
		default:
			break;
	}
}


Tile::~Tile()
{
	
}


void Tile::setPosition(int x, int y)
{
	this->position.x = x;
	this->position.y = y;
}


bool Tile::print_onMap(SDL_Point offset)
{
	SDL_Rect offsetRect;
	bool retval;

	offsetRect.x = offset.x + (this->position.x*TILESIZE);
	offsetRect.y = offset.y + (this->position.y*TILESIZE);
	offsetRect.w = dst_rect.w;
	offsetRect.h = dst_rect.h;

	retval = this->print(NULL, &offsetRect);

	if ((mouse.x < (offsetRect.x + offsetRect.w)) & (mouse.x > offsetRect.x))
	{
		if ((mouse.y < (offsetRect.y + offsetRect.h)) & (mouse.y > offsetRect.y))
		{
			if (mouse.left)
				SDL_SetRenderDrawColor(render, 32, 255, 32, 64);
			else
				SDL_SetRenderDrawColor(render, 255, 255, 255, 64);
			SDL_RenderFillRect(render, &offsetRect);
		}
	}

	return (retval);
}


void Tile::load_all_texture()
{
	ifstream	config("config/tiles.config");
	string		line;
	int			index;

	Tile_type tileType;
	string    file;

	console->log("Load tiles");

	/* reset tile */
	for (int i = 0; i < Tile_type::LAST_TTYPE; i++)
		Tile::texture[i] = NULL;
	

	while (getline(config, line) )
	{
		if (line.c_str()[0] == '#')
			continue;

		tileType = Tile_type::LAST_TTYPE;
		file 	 = "";

		index = line.find(";");
		if(index == (int)string::npos)
			continue;
		
		string str_tileType = line.substr(0, index);

		if (str_tileType == "DIRT")
			tileType = Tile_type::DIRT;
		if (str_tileType == "WATER")
			tileType = Tile_type::WATER;
		if (str_tileType == "STONE")
			tileType = Tile_type::STONE;
		if (str_tileType == "BUSH")
			tileType = Tile_type::BUSH;

		file = line.substr(index+1, line.length());

		file.insert(0, "textures/");
		
		Tile::texture[tileType] = createTexture(NULL, file.c_str());
	}

	config.close();
}


void Tile::unload_all_texture()
{
	console->log("Unload tile");

	for (int i = 0; i < Tile_type::LAST_TTYPE; i++)
	{
		if (Tile::texture[i] != NULL)
			SDL_DestroyTexture(Tile::texture[i]);
	}
	
}


Entity::Entity(const char *entityName, const char *texturePath) : Printable(entityName, texturePath)
{
	this->position.x = 0;
	this->position.y = 0;

	this->positionScreen.x = this->position.x*TILESIZE;
	this->positionScreen.y = this->position.y*TILESIZE;
	
	this->src_rect.x = 0;
	this->src_rect.y = 0;
	this->src_rect.w = screen.w;
	this->src_rect.h = screen.h;

	this->dst_rect.x = this->positionScreen.x;
	this->dst_rect.y = this->positionScreen.y;
	this->dst_rect.w = TILESIZE;
	this->dst_rect.h = TILESIZE;

	this->moving = Direction::NONE;
}


Entity::~Entity()
{
}


void Entity::move(Direction direction, Tile *tile)
{
	if (tile == NULL)
		return;
	if ((direction == tile->getWalkable()) || (tile->getWalkable() == Direction::ALL))
	{
		switch (direction)
		{
			case Direction::NORTH:
				this->position.y -= 1;
				break;
			case Direction::SOUTH: 
				this->position.y += 1;
				break;
			case Direction::WEST:  
				this->position.x -= 1;
				break;
			case Direction::EAST:  
				this->position.x += 1;
				break;

			default:
				break;
		}

		this->moving = direction;
	}
}

void Entity::proc(void)
{
	if (this->moving != Direction::NONE)
	{
		switch (this->moving)
		{
			case Direction::NORTH: 
				this->positionScreen.y -= ENTITYSPEED;
				break;
			case Direction::SOUTH: 
				this->positionScreen.y += ENTITYSPEED;
				break;
			case Direction::WEST:  
				this->positionScreen.x -= ENTITYSPEED;
				break;
			case Direction::EAST:  
				this->positionScreen.x += ENTITYSPEED;
				break;

			default:
				break;
		}

		if ((this->positionScreen.x == this->position.x*TILESIZE) && (this->positionScreen.y == this->position.y*TILESIZE))
			this->moving = Direction::NONE;
	
		console->log("player : x%d y%d tile : x%d y%d ", this->positionScreen.x, this->positionScreen.y, this->position.x*TILESIZE, this->position.y*TILESIZE);
	}

	/* TODO PSN, EMBUSH */
}

bool Entity::print_onMap(SDL_Point offset)
{
	SDL_Rect offsetRect;
	offsetRect.x = offset.x + this->positionScreen.x;
	offsetRect.y = offset.y + this->positionScreen.y;
	offsetRect.w = dst_rect.w;
	offsetRect.h = dst_rect.h;

	return (this->print(&this->src_rect, &offsetRect));
}
