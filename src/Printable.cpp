#include "Printable.hpp"
#include <fstream>

#include "Console.hpp"
#include "EventManager.hpp"
#include "Map.hpp"

/* extern var including */

extern Console		*console;
extern SDL_Renderer *render;
extern SDL_Rect		screen;
extern Mouse_t		mouse;
extern EventManager	*event;

/* static var creation */

bool				Printable::debug = false;
int 				Printable::tilesize = 64;
vector<Printable *> Printable::toDebug; 
SDL_Texture			*Tile::texture[Tile_type::LAST_TTYPE];
TTF_Font			*Text::fonts[Font_type::LAST_FONT];
NPC					*NPC::allNPC[MAX_NPC];
Entity				*Entity::allEntity[MAX_ENTITY];
string 				NPC::history[MAX_DIALOG];


SDL_Texture *createTexture(SDL_Rect *rectangle, const char *path)
{
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	surface = IMG_Load(path);

	if (surface)
	{
		texture = SDL_CreateTextureFromSurface(render, surface);
		SDL_FreeSurface(surface);

		if(rectangle != NULL)
			SDL_QueryTexture(texture, NULL, NULL, &rectangle->w, &rectangle->h);

		return (texture);
	}
	else
	{
		console->log(ERROR, "Create texture : \"%s\" : %s", path, SDL_GetError());
	}
	return (NULL);
}


TTF_Font *createFont(const char *path, int size)
{
	TTF_Font *font;
	font = TTF_OpenFont(path, size);

	if (!font)
	{
		console->log(ERROR, "Opening font (%s) : %s", path, SDL_GetError());
	}
	
	return (font);
}


SDL_Texture *write(SDL_Rect *rect, TTF_Font *font, const char *text, SDL_Color color)
{
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	surface = TTF_RenderText_Solid(font, text, color);

	texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_FreeSurface(surface);

	if (texture == NULL) 
	{
		console->log(ERROR, "Create texte texture : texture NULL");
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
	}

	return (texture);
}


/*
 ************************************************************************************************
 *		Printable class																			*
 *																								*
 ************************************************************************************************
 */

Printable::Printable(SDL_Rect size)
{
	char printableName[32]; 
	sprintf(printableName, "[Printable] 0x%x", (unsigned int)this);

	this->name.assign(printableName);
	this->path.assign("");

	/* texture ptr should be set after */
	this->texture = NULL;

	this->destroy_texture = false;

	this->dst_rect.x = size.x;
	this->dst_rect.y = size.y;
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


void Printable::setAnimation(int n_frames, int ttl, SDL_Rect size)
{
	this->animated			= true;

	if (ttl == -1)
	{
		ttl = ANIMATION_SPEED / n_frames;
	}

	this->nb_frames			= n_frames;
	this->frame_ttl			= ttl;

	this->current_frame		= 0;
	this->current_time		= 0;
	this->current_animation	= 0;
	
	this->src_rect.x		= size.x;
	this->src_rect.y		= size.y;
	this->src_rect.w		= size.w;
	this->src_rect.h		= size.h;
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

	if (this->animated)
	{
		src->x = src_rect.w * this->current_animation;
		src->y = src_rect.h * this->current_frame;

		if (current_time == 0)
		{
			this->current_frame = (this->current_frame == this->nb_frames-1) ? (0) : (this->current_frame + 1);
			this->current_time = this->frame_ttl;
		}
		else
		{
			this->current_time--;
		}

	}

	retval = SDL_RenderCopy(render, this->texture, src, dst);

	if (retval == 0)
	{
		return (true);
	}
	else
	{
		console->log(ERROR, "Cannot print \"%s\" %s", this->name.c_str(), SDL_GetError());
		return (false);
	}
}


void Printable::print_debug(void)
{

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


void Printable::proc_debug(void)
{
	if (Printable::debug)
	{
		for (unsigned int i = 0; i < Printable::toDebug.size(); i++)
		{
			Printable::toDebug[i]->print_debug();
		}
	}
	Printable::toDebug.clear();
}


/*
 ************************************************************************************************
 *		Tile class																				*
 *																								*
 ************************************************************************************************
 */

Tile::Tile(Tile_params params) : Printable({params.x, params.y, Printable::tilesize, Printable::tilesize})
{
	this->setPosition(params.x, params.y);

	this->walkable = params.walkable;

	this->type = params.type;

	/* if unknow type */
	if (!((this->type != Tile_type::EMPTY) && (this->type >= 0) && (this->type < Tile_type::LAST_TTYPE)))	
	{
		this->type = Tile_type::DIRT;
	}

	this->setTexture(Tile::texture[this->type]);
}


Tile::Tile(Tile_type tiletype, SDL_Rect info) : Printable(info)
{
	this->position.x = info.x;
	this->position.y = info.y;

	this->type = tiletype;

	/* if unknow type */
	if (!((this->type != Tile_type::EMPTY) && (this->type >= 0) && (this->type < Tile_type::LAST_TTYPE)))	
	{
		this->type = Tile_type::DIRT;
	}

	this->setTexture(Tile::texture[this->type]);
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

	offsetRect.x = offset.x + (this->position.x*Printable::tilesize);
	offsetRect.y = offset.y + (this->position.y*Printable::tilesize);
	offsetRect.w = this->dst_rect.w;
	offsetRect.h = this->dst_rect.h;

	retval = this->print(NULL, &offsetRect);

	if ((mouse.x < (offsetRect.x + offsetRect.w)) & (mouse.x > offsetRect.x))
	{
		if ((mouse.y < (offsetRect.y + offsetRect.h)) & (mouse.y > offsetRect.y))
		{
			if (mouse.left)
				SDL_SetRenderDrawColor(render, 64, 64, 64, 64);
			else
				SDL_SetRenderDrawColor(render, 255, 255, 255, 64);

			SDL_RenderFillRect(render, &offsetRect);

			if (this->debug)
			{
				Printable::toDebug.push_back(this);
			}
		}
	}

	return (retval);
}


void Tile::print_debug(void)
{
	SDL_Rect advancedInfo;

	char posbuffer[512];
	char typbuffer[64];
	char wlkbuffer[64];

	int lenght = 0;

	sprintf(posbuffer, "[Pos]: %2d %2d", this->position.x, this->position.y);

	switch (this->type)
	{
		case DIRT:			 sprintf(typbuffer, "[Type]: DIRT");			break;
		case DIRT_PATH_1:	 sprintf(typbuffer, "[Type]: DIRT_PATH_1");		break;
		case DIRT_PATH_2:	 sprintf(typbuffer, "[Type]: DIRT_PATH_2");		break;
		case DIRT_PATH_3:	 sprintf(typbuffer, "[Type]: DIRT_PATH_3");		break;
		case DIRT_PATH_4:	 sprintf(typbuffer, "[Type]: DIRT_PATH_4");		break;
		case DIRT_NE_GRASS:	 sprintf(typbuffer, "[Type]: DIRT_NE_GRASS");	break;
		case DIRT_NW_GRASS:	 sprintf(typbuffer, "[Type]: DIRT_NW_GRASS");	break;
		case DIRT_SE_GRASS:	 sprintf(typbuffer, "[Type]: DIRT_SE_GRASS");	break;
		case DIRT_SW_GRASS:	 sprintf(typbuffer, "[Type]: DIRT_SW_GRASS");	break;
		case GRASS_E_W_DIRT: sprintf(typbuffer, "[Type]: GRASS_E_W_DIRT");	break;
		case GRASS_N_S_DIRT: sprintf(typbuffer, "[Type]: GRASS_N_S_DIRT");	break;
		case GRASS_NE_DIRT:	 sprintf(typbuffer, "[Type]: GRASS_NE_DIRT");	break;
		case GRASS_NW_DIRT:	 sprintf(typbuffer, "[Type]: GRASS_NW_DIRT");	break;
		case GRASS_S_N_DIRT: sprintf(typbuffer, "[Type]: GRASS_S_N_DIRT");	break;
		case GRASS_SE_DIRT:	 sprintf(typbuffer, "[Type]: GRASS_SE_DIRT");	break;
		case GRASS_SW_DIRT:	 sprintf(typbuffer, "[Type]: GRASS_SW_DIRT");	break;
		case GRASS_W_E_DIRT: sprintf(typbuffer, "[Type]: GRASS_W_E_DIRT");	break;
		case GRASS_1:		 sprintf(typbuffer, "[Type]: GRASS_1");			break;
		case GRASS_2:		 sprintf(typbuffer, "[Type]: GRASS_2");			break;
		case GRASS_3:		 sprintf(typbuffer, "[Type]: GRASS_3");			break;
		case TINYBUSH_1:	 sprintf(typbuffer, "[Type]: TINYBUSH_1");		break;
		case TINYBUSH_2:	 sprintf(typbuffer, "[Type]: TINYBUSH_2");		break;
		case TINYBUSH_3:	 sprintf(typbuffer, "[Type]: TINYBUSH_3");		break;
		case TINYBUSH_4:	 sprintf(typbuffer, "[Type]: TINYBUSH_4");		break;
	
		default: 	sprintf(typbuffer, "[Type]: Unknow");
	}

	
	switch (this->walkable)
	{
		case Direction::NORTH:	sprintf(wlkbuffer, "[Walkable]: NORTH");	break;
		case Direction::SOUTH:	sprintf(wlkbuffer, "[Walkable]: SOUTH");	break;
		case Direction::WEST:	sprintf(wlkbuffer, "[Walkable]: WEST");		break;
		case Direction::EAST:	sprintf(wlkbuffer, "[Walkable]: EAST");		break;
		case Direction::ALL:	sprintf(wlkbuffer, "[Walkable]: ALL");		break;

		default: 	sprintf(typbuffer, "[Walkable]: Unknow");
	}
	
	
	advancedInfo.w = 200;
	advancedInfo.h = 100;
	advancedInfo.x = mouse.x + 4;
	advancedInfo.y = mouse.y - advancedInfo.h - 4;

	Text posText(posbuffer, Font_type::AVARA, {advancedInfo.x+4, advancedInfo.y+4});
	Text typText(typbuffer, Font_type::AVARA, {advancedInfo.x+4, advancedInfo.y+4+posText.getHitbox().h});//typText.getHitbox().h+5});
	Text wlkText(wlkbuffer, Font_type::AVARA, {advancedInfo.x+4, advancedInfo.y+8+2*posText.getHitbox().h});//typText.getHitbox().h+5});

	if (lenght < posText.getHitbox().w)
		lenght = posText.getHitbox().w;
	if (lenght < typText.getHitbox().w)
		lenght = typText.getHitbox().w;
	if (lenght < wlkText.getHitbox().w)
		lenght = wlkText.getHitbox().w;
		
	advancedInfo.w = lenght + 8;

	SDL_SetRenderDrawColor(render, 128, 128, 128, 200);
	SDL_RenderFillRect(render, &advancedInfo);

	posText.print_onMap();
	typText.print_onMap();
	wlkText.print_onMap();
}


Tile_type Tile::getTiletype(string str)
{
	Tile_type type = Tile_type::EMPTY;

	if (str == "DIRT")
		type = Tile_type::DIRT;
	else if (str == "DIRT_PATH_1")
		type = Tile_type::DIRT_PATH_1;
	else if (str == "DIRT_PATH_2")
		type = Tile_type::DIRT_PATH_2;
	else if (str == "DIRT_PATH_3")
		type = Tile_type::DIRT_PATH_3;
	else if (str == "DIRT_PATH_4")
		type = Tile_type::DIRT_PATH_4;
	else if (str == "DIRT_NE_GRASS")
		type = Tile_type::DIRT_NE_GRASS;
	else if (str == "DIRT_NW_GRASS")
		type = Tile_type::DIRT_NW_GRASS;
	else if (str == "DIRT_SE_GRASS")
		type = Tile_type::DIRT_SE_GRASS;
	else if (str == "DIRT_SW_GRASS")
		type = Tile_type::DIRT_SW_GRASS;
	else if (str == "GRASS_E_W_DIRT")
		type = Tile_type::GRASS_E_W_DIRT;
	else if (str == "GRASS_N_S_DIRT")
		type = Tile_type::GRASS_N_S_DIRT;
	else if (str == "GRASS_NE_DIRT")
		type = Tile_type::GRASS_NE_DIRT;
	else if (str == "GRASS_NW_DIRT")
		type = Tile_type::GRASS_NW_DIRT;
	else if (str == "GRASS_S_N_DIRT")
		type = Tile_type::GRASS_S_N_DIRT;
	else if (str == "GRASS_SE_DIRT")
		type = Tile_type::GRASS_SE_DIRT;
	else if (str == "GRASS_SW_DIRT")
		type = Tile_type::GRASS_SW_DIRT;
	else if (str == "GRASS_W_E_DIRT")
		type = Tile_type::GRASS_W_E_DIRT;
	else if (str == "GRASS_1")
		type = Tile_type::GRASS_1;
	else if (str == "GRASS_2")
		type = Tile_type::GRASS_2;
	else if (str == "GRASS_3")
		type = Tile_type::GRASS_3;
	else if (str == "TINYBUSH_1")
		type = Tile_type::TINYBUSH_1;
	else if (str == "TINYBUSH_2")
		type = Tile_type::TINYBUSH_2;
	else if (str == "TINYBUSH_3")
		type = Tile_type::TINYBUSH_3;
	else if (str == "TINYBUSH_4")
		type = Tile_type::TINYBUSH_4;

	return (type);
}

Direction Tile::getTileDir(string str)
{
	Direction dir = Direction::NONE;
	
	if (str == "NORTH")
		dir = Direction::NORTH;
	else if (str == "SOUTH")
		dir = Direction::SOUTH;
	else if (str == "WEST")
		dir = Direction::WEST;
	else if (str == "EAST")
		dir = Direction::EAST;
	else if (str == "ALL")
		dir = Direction::ALL;
	else if (str == "NONE")
		dir = Direction::NONE;

	return (dir);
}

void Tile::load_all_texture()
{
	ifstream	config("config/tiles.cnf");
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

		tileType = getTiletype(str_tileType);

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


/*
 ************************************************************************************************
 *		Entity class																			*
 *																								*
 ************************************************************************************************
 */

Entity::Entity(const char *entityName, const char *texturePath) : Printable(entityName, texturePath)
{
	for (int i = 0; i < MAX_ENTITY; i++)
	{
		if (NPC::allEntity[i] == NULL)
		{
			this->i_ent = i;
			NPC::allEntity[i] = this;
			break;
		}
	}

	this->position.x = 0;
	this->position.y = 0;

	this->positionScreen.x = 0;
	this->positionScreen.y = 0;
	
	this->src_rect.x = 0;
	this->src_rect.y = 0;
	this->src_rect.w = this->getHitbox().w;
	this->src_rect.h = this->getHitbox().h;

	this->dst_rect.x = this->positionScreen.x;
	this->dst_rect.y = this->positionScreen.y;
	this->dst_rect.w = Printable::tilesize;
	this->dst_rect.h = Printable::tilesize;

	this->moving = Direction::NONE;
}


Entity::~Entity()
{
	NPC::allEntity[this->i_ent] = NULL;
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
				direction = Direction::NONE;
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

		if ((this->positionScreen.x == this->position.x*Printable::tilesize) && (this->positionScreen.y == this->position.y*Printable::tilesize))
		{
			this->moving = Direction::NONE;
		}

	}

	/* TODO EMBUSH, DIALOG*/
}


bool Entity::print_onMap(SDL_Point offset)
{
	SDL_Rect offsetRect;
	offsetRect.x = offset.x + this->positionScreen.x;
	offsetRect.y = offset.y + this->positionScreen.y;
	offsetRect.w = this->dst_rect.w;
	offsetRect.h = this->dst_rect.h;

	switch (this->moving)
	{
		case Direction::NORTH: 
			this->current_animation = 1;
			break;
		case Direction::SOUTH: 
			this->current_animation = 0;
			break;
		case Direction::WEST:  
			this->current_animation = 2;
			break;
		case Direction::EAST:  
			this->current_animation = 3;
			break;
			
		default:
			if (this->current_frame == 0)
				this->current_time	= 1;
			break;
	}

	return (this->print(&this->src_rect, &offsetRect));
}


void Entity::print_debug(void)
{

}


/*
 ************************************************************************************************
 *		Waifu class																				*
 *																								*
 ************************************************************************************************
 */

Waifu::Waifu(Waifu_params params) : Entity(params.name, params.texture_path)
{

}


Waifu::~Waifu()
{

}


/*
 ************************************************************************************************
 *		NPC class																				*
 *																								*
 ************************************************************************************************
 */

NPC::NPC(const char *entityName, const char *texturePath) : Entity(entityName, texturePath)
{
	this->load(NULL);
}


NPC::NPC(NPC_params params) : Entity(params.name, params.path)
{
	this->load(&params);
}


void NPC::load(NPC_params *params)
{
	for (int i = 0; i < MAX_NPC; i++)
	{
		if (NPC::allNPC[i] == NULL)
		{
			this->i_npc = i;
			NPC::allNPC[i] = this;
			break;
		}
	}

	for (int cpt = 0; cpt < NPC_MAX_DIALOG; cpt++)
	{
		this->dialogs[cpt] = -69;
	}

	if (params != NULL)
	{
		this->setPosition(params->x, params->y);
	}

	// console->log("new NPC num: %d at 0x%x", this->i_npc, NPC::allNPC[this->i_npc]);

}


NPC::~NPC()
{
	NPC::allNPC[this->i_npc] = NULL;
}


NPC	*NPC::getNPC(SDL_Point pos)
{
	NPC *npc = NULL;

	for (int i = 0; i < MAX_NPC; i++)
	{
		if (NPC::allNPC[i] != NULL)
		{
			if ((NPC::allNPC[i]->getPosition().x == pos.x) && (NPC::allNPC[i]->getPosition().y == pos.y))
			{
				npc = NPC::allNPC[i];
				break;
			}
		}
	}
	
	return (npc);
}


void NPC::addDialog(int dialog)
{
	if (this->nb_dialog < NPC_MAX_DIALOG)
		this->dialogs[this->nb_dialog] = dialog;

	this->nb_dialog++;
}


bool NPC::hasDialog(int hist)
{
	bool retval = false;

	for (int i_hist = 0; i_hist < NPC_MAX_DIALOG; i_hist++)
	{
		if (this->dialogs[i_hist] == hist)
		{
			retval = true;
			break;
		}
	}

	return (retval);
}


void NPC::proc_print(SDL_Point offset)
{
	for (int i = 0; i < MAX_NPC; i++)
	{
		if (NPC::allNPC[i] != NULL)
		{
			NPC::allNPC[i]->print_onMap(offset);
		}
	}
}


void NPC::load_history(void)
{
	ifstream	config("config/history.cnf");
	string		line;
	int			index;
	int 		dialog_idx;
	int			history_lenght = 0;

	console->log("Load history");

	/* reset dialogs */
	for (int i = 0; i < MAX_DIALOG; i++)
		NPC::history[i].assign("");


	while (getline(config, line) )
	{
		if (line.c_str()[0] == '#')
			continue;

		index = line.find(";");

		/* if error continue */
		if(index == (int)string::npos)
		{
			console->log(log_t::ERROR, "Load dialogs : \"%s\" no \';\'", line.c_str());
			continue;
		}

		string str_dialog_idx = line.substr(0, index);
		dialog_idx = atoi(str_dialog_idx.c_str());
		
		if  (dialog_idx == 0)
		{
			console->log(log_t::ERROR, "Conversion to integer of : \"%s\"", str_dialog_idx.c_str());
			continue;
		}
		
		dialog_idx--; /* to make it start from 0 and not 1 */

		NPC::history[dialog_idx].assign(line.substr(index+1, line.length()) );
		
		history_lenght++;
//		console->log("Set dialog %d : %s", dialog_idx, NPC::history[dialog_idx].c_str());
	}

	console->log("Succesfully load %d dialog%s", history_lenght, (history_lenght <= 1) ? "" : "s");

	config.close();
}


void NPC::load_all(string mapname)
{
	ifstream	config("config/NPC.cnf");
	string		line;
	int 		nb_NPC = 0;
	bool 		mapOK  = false;

	mapname = "[" + mapname + "]";

	console->log("Load NPC (map: %s)", mapname.c_str());

	/* reset entity */
	NPC::unload_all();
	
	while (getline(config, line) )
	{
		if (line.c_str()[0] == '#')
			continue;

		if (line.c_str()[0] == '[')
		{
			if (line == mapname)
				mapOK = true;
			else
				mapOK = false;

			continue;
		}

		if (mapOK)
		{
			NPC_params params;
			string str_name, str_path, str_posX, str_posY;
			int	l_idx = 0;
			int r_idx = 0;
			int dialogsIdx = 0;
			int nb_dialogsIdx = 0;

			r_idx = line.find(";");
			str_name = line.substr(l_idx, r_idx - l_idx);

			l_idx = r_idx + 1;
			r_idx = line.find(";", l_idx);
			str_path = line.substr(l_idx, r_idx - l_idx);

			l_idx = r_idx + 1;
			r_idx = line.find(";", l_idx);
			str_posX = line.substr(l_idx, r_idx - l_idx);
			
			l_idx = r_idx + 1;
			r_idx = line.find(";", l_idx);
			str_posY = line.substr(l_idx, r_idx - l_idx);

			SDL_strlcpy(params.name, str_name.c_str(), 64);
			SDL_strlcpy(params.path, str_path.c_str(), 64);
			params.x = atoi(str_posX.c_str());
			params.y = atoi(str_posY.c_str());

			// console->log("name:%s path:%s x:%d y:%d", params.name, params.path, params.x, params.y);

			NPC *npc = new NPC(params);

			nb_NPC++;

			/* dialog generation */

			l_idx = r_idx + 1;
			r_idx = line.find(";", l_idx);
			while (r_idx != (int)string::npos)
			{
				string strDialog;

				if (l_idx > r_idx)
				{
					console->log(log_t::ERROR, "\tInvalid index l_idx > r_idx (%s > %s)", l_idx, r_idx);
					return;
				}

				strDialog  = line.substr(l_idx, r_idx - l_idx);
				dialogsIdx = atoi(strDialog.c_str());

				nb_dialogsIdx++;

				npc->addDialog(dialogsIdx);

				l_idx = r_idx + 1;
				r_idx = line.find(";", l_idx);
			}

			// console->log("Found %d dialog%s", nb_dialogsIdx, (nb_dialogsIdx <= 1) ? "" : "s");
		}
	}

	console->log("Succesfully load %d NPC%s (map: %s)", nb_NPC, (nb_NPC <= 1) ? "" : "s", mapname.c_str());

	config.close();
}


void NPC::unload_all(void)
{
	for (int i = 0; i < MAX_NPC; i++)
	{
		if (NPC::allNPC[i] != NULL)
			delete (NPC::allNPC[i]);
	}
}


/*
 ************************************************************************************************
 *		Player class																			*
 *																								*
 ************************************************************************************************
 */

Player::Player(const char *entityName, const char *texturePath) : Entity(entityName, texturePath)
{
	Waifu_params wfu_params;
	for (int i_wfu = 0; i_wfu < DECK_SIZE; i_wfu++)
	{
		sprintf(wfu_params.name, "TWO_B");
		sprintf(wfu_params.texture_path, "img/entity/mobTest.png");
		wfu_params.type = Waifu_type::TWO_B;
		
		this->deck[i_wfu] = new Waifu(wfu_params);
	}
	this->dialogTarget = NULL;
	this->inDialog = false;	
	this->history = 0;
}


Player::~Player()
{

}

void Player::proc(int *ptr_map)
{
	Map *map = (Map *)ptr_map;
	static int32_t ignoreMove = 0;

	/* deplacement proc */
	if (this->canMove() && !this->inDialog)
	{
		SDL_Point futurePos = this->position;

		if (event->getKeyK(SDLK_z) )
		{
			if (this->orientation != Direction::NORTH)
			{
				ignoreMove = IGNORE_DELAY;
				this->orientation = Direction::NORTH;
			}

			if (ignoreMove < 0)
			{
				futurePos.y--;
				this->move(Direction::NORTH, map->getTile(futurePos));
			}
		}
		else if (event->getKeyK(SDLK_s) )
		{
			if (this->orientation != Direction::SOUTH)
			{
				ignoreMove = IGNORE_DELAY;
				this->orientation = Direction::SOUTH;
			}

			if (ignoreMove < 0)
			{
				futurePos.y++;
				this->move(Direction::SOUTH, map->getTile(futurePos));
			}
		}
		else if (event->getKeyK(SDLK_q) )
		{
			if (this->orientation != Direction::WEST)
			{
				ignoreMove = IGNORE_DELAY;
				this->orientation = Direction::WEST;
			}

			if (ignoreMove < 0)
			{
				futurePos.x--;
				this->move(Direction::WEST, map->getTile(futurePos));
			}
		}
		else if (event->getKeyK(SDLK_d) )
		{
			if (this->orientation != Direction::EAST)
			{
				ignoreMove = IGNORE_DELAY;
				this->orientation = Direction::EAST;
			}

			if (ignoreMove < 0)
			{
				futurePos.x++;
				this->move(Direction::EAST, map->getTile(futurePos));
			}
		}

		ignoreMove--;
	}
	else
	{
		if (this->moving == Direction::NORTH || 
			this->moving == Direction::SOUTH ||
			this->moving == Direction::EAST  ||
			this->moving == Direction::WEST)
		{
			if (this->orientation != this->moving)
			{
//				this->orientation = this->moving;
				console->log("Orientation changed");
			}
		}

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

		if ((this->positionScreen.x == this->position.x*Printable::tilesize) && (this->positionScreen.y == this->position.y*Printable::tilesize))
		{
			this->moving = Direction::NONE;
		}
	}

	/* dialog proc */		
	if (event->getKeyUp(SDL_GetScancodeFromKey(SDLK_e)) )
	{
		SDL_Point	talkingPos = this->position;
		NPC			*npcTarget;
		
		switch (this->orientation)
		{
			case Direction::NORTH: talkingPos.y -= 1; break;
			case Direction::SOUTH: talkingPos.y += 1; break;
			case Direction::WEST:  talkingPos.x -= 1; break;
			case Direction::EAST:  talkingPos.x += 1; break;

			default:
				console->log("Dialog test on player pos");
				break;
		}

		npcTarget = NPC::getNPC(talkingPos);

		if (!this->inDialog)
		{
			if (npcTarget != NULL)
			{
				if (npcTarget->hasDialog(this->history) )
				{
					this->dialogText	= NPC::history[this->history];
					this->dialogTarget	= npcTarget;

					this->history++;
				}
				else
				{
					if (npcTarget->hasDialog(this->history-1))
					{
						this->dialogText	= NPC::history[this->history-1];
						this->dialogTarget	= npcTarget;	
					}
					else
					{
						this->dialogText = npcTarget->rdmDialogs[rand() % SDL_arraysize(npcTarget->rdmDialogs)];
					}
					
				}

				this->inDialog = true;

			}

		}
		else
		{
			if (npcTarget != NULL && npcTarget->hasDialog(this->history) )
			{
				this->dialogText	= NPC::history[this->history];
				this->dialogTarget	= npcTarget;

				this->i_dglChar = 0;	/* reload cpt */

				this->history++;

				this->inDialog = true;
			}
			else
			{
				this->inDialog = false;
			}
		}

		if (this->inDialog)
			console->log("dialog : %s", this->dialogText.c_str());

	}



	/* TODO PSN, EMBUSH, DIALOG */
}


bool Player::print_onMap(SDL_Point offset)
{
	SDL_Rect offsetRect;
	offsetRect.x = offset.x + this->positionScreen.x;
	offsetRect.y = offset.y + this->positionScreen.y;
	offsetRect.w = this->dst_rect.w;
	offsetRect.h = this->dst_rect.h;

	switch (this->orientation)
	{
		case Direction::NORTH: this->current_animation = 1; break;
		case Direction::SOUTH: this->current_animation = 0; break;
		case Direction::WEST:  this->current_animation = 2; break;
		case Direction::EAST:  this->current_animation = 3; break;
			
		default:
			if (this->current_frame == 0)
				this->current_time	= 1;
			break;
	}

	if (this->moving == Direction::NONE)
	{
		if (this->current_frame == 0)
			this->current_time	= 1;
	}

	/* display dialog */
	if (this->inDialog)
	{
		SDL_Rect textArea;
		string ttp;
		Text *printedText;
		
		textArea.h = 100;
		textArea.w = screen.w - 20;
		textArea.x = 10;
		textArea.y = screen.h - textArea.h - 10;

		SDL_SetRenderDrawColor(render, 128, 128, 128, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(render, &textArea);

		if (this->i_dglChar < this->dialogText.length())
		{
			this->i_dglChar++;
		}

		ttp = this->dialogText.substr(0, this->i_dglChar);
	
		printedText = new Text(ttp.c_str(), Font_type::AVARA, {textArea.x + 10, textArea.y + 10});

		printedText->print_onMap();

		delete (printedText);
 	}
	else
	{
		this->i_dglChar = 0;
	}

	return (this->print(&this->src_rect, &offsetRect));
}




/*
 ************************************************************************************************
 *		Text class																				*
 *																								*
 ************************************************************************************************
 */

Text::Text(const char *content, Font_type fontype, SDL_Point pos) : Printable((SDL_Rect){pos.x, pos.y, Printable::tilesize, Printable::tilesize})
{
	SDL_Rect	rect;
	this->text.assign(content);
	TTF_Font	*textfont	= Text::fonts[fontype];
	SDL_Texture *texture	= write(&rect, textfont, this->text.c_str(), {0, 0, 0});

	this->setTexture(texture);
	this->setHitbox();

	this->dst_rect.x = pos.x;
	this->dst_rect.y = pos.y;
	this->dst_rect.h = this->getHitbox().h;
	this->dst_rect.w = this->getHitbox().w;

	this->destroy_texture = true;
}


Text::~Text()
{

}


bool Text::print_onMap(SDL_Point offset)
{
	SDL_Rect offsetRect;

	offsetRect.x = dst_rect.x + offset.x;
	offsetRect.y = dst_rect.y + offset.y;
	offsetRect.w = this->dst_rect.w;
	offsetRect.h = this->dst_rect.h;

	return (this->print(NULL, &offsetRect));
}


void Text::load_font(void)
{
	console->log("Load fonts");
	
	Text::fonts[Font_type::AVARA]	= createFont("Avara.ttf", 16);
	Text::fonts[Font_type::DUNO]	= NULL;
}


void Text::unload_font(void)
{
	console->log("Unload fonts");

	for (int i = 0; i < Font_type::LAST_FONT; i++)
	{
		if (Text::fonts[i] != NULL)
			TTF_CloseFont(fonts[i]);
	}
}

