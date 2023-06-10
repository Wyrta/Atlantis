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

#define IGNORE_DELAY	10
#define ENTITYSPEED		2

#define DECK_SIZE		6
#define MAX_SPELL		4
#define SPELLBOOK_SIZE	128
#define MAX_WAIFU		64

#define ANIMATION_SPEED	32

#define MAX_DIALOG		64
#define NPC_MAX_DIALOG	16

#define MAX_NPC			64
#define MAX_ENTITY		(MAX_NPC+1)
#define MAX_BUTTON		32

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


typedef enum {
	TXT,
	IMG,
	NON
} Button_type;


typedef struct {
	int x;
	int y;

	Tile_type type;
	
	Direction walkable;
} Tile_params;

typedef struct {
	int power;
	int speed;
	int precision;

	std::string name;
	std::string type;
} Spell;

typedef enum {
	TWO_B,
	A_TWO,
	NINE_S,
	KAINE,
	NIER,

	LAST_WTYPE
} Waifu_type;


typedef struct {
	char name[64];
	char texture_path[512];

	Waifu_type type;
	int lvl;
	int xp;
} Waifu_params;

typedef struct {
	int x;
	int y;

	char name[64];

	char path[512];
} NPC_params;



class Printable
{
	private:
		SDL_Texture	*texture = NULL;
		SDL_Rect	texture_rect;

	protected:
		std::string	name = "";
		std::string	path = "";

		SDL_Point position;
		SDL_Point positionTile;

		SDL_Rect	src_rect;
		SDL_Rect	dst_rect;

		bool		animated = false;
		int 		nb_frames;
		int 		frame_ttl;

		int			current_frame;
		int			current_time;
		int			current_animation;

		static vector<Printable *> toDebug;
		static int 	stack_debug;
		bool 		destroy_texture = true;

	public:
		Printable(SDL_Rect size);
		Printable(const char *objname, const char *filepath);
		virtual ~Printable();

		void		setAnimation(int n_frames, int ttl, SDL_Rect size);

		static bool debug;
		static void proc_debug(void);

		bool		print(void);
		bool		print(SDL_Rect *src_rect, SDL_Rect *dst_rect);
		bool		print(SDL_Point pos);
		virtual void print_debug(void);
		virtual void print_onMap(SDL_Point offset);

		void		setTexture(const char *filepath);
		void		setTexture(SDL_Texture *ptr_texture);
		SDL_Rect	getHitbox(void) { return this->texture_rect; }
		void		setHitbox(void) { SDL_QueryTexture(this->texture, NULL, NULL, &this->texture_rect.w, &this->texture_rect.h); }

		static int 	tilesize;

};


class Tile : public Printable
{
	private:
		static SDL_Texture *texture[Tile_type::LAST_TTYPE];

		Tile_type type;
		Direction walkable;

	public:
	
		Tile(Tile_params params);
		Tile(Tile_type tiletype = EMPTY, SDL_Rect info = {0, 0, Printable::tilesize , Printable::tilesize });
		~Tile();

		void		print_onMap(SDL_Point offset);
		void		print_debug(void);

		void		setPosition(int x, int y);

		SDL_Point	getPosition(void)	{ return (this->positionTile); }
		Direction	getWalkable(void) 	{ return (this->walkable); }

		static Tile_type getTiletype(string str);
		static Direction getTileDir(string str);

		static void load_all_texture(void);
		static void unload_all_texture(void);
};


class Waifu : public Printable
{
	private:
		int hp;
		int atk;
		int def;
		int speed;

		int spellsID[MAX_SPELL];
	public:
		Waifu(Waifu_params params);
		~Waifu();

		bool canFight(void);
		string attack(Waifu *target, int spell_num);
		
		static Spell	spellsBook[SPELLBOOK_SIZE];
		static Waifu	*bestiary[MAX_WAIFU];
		static void		load_all(void);
		static void		unload_all(void);
};


class Entity : public Printable
{
	private:

	protected:
		Direction	moving;
		Direction	orientation;
		Waifu		*deck[DECK_SIZE];

	public:
		Entity(const char *entityName, const char *texturePath);
		~Entity();

		void			print_onMap(SDL_Point offset);
		void			print_debug(void);

		virtual void	proc(void);

		static Entity	*allEntity[MAX_ENTITY];
		int				i_ent;

		void			move(Direction direction, Tile *tile);

		SDL_Point		getPosition(void)			{ return this->positionTile; }
		SDL_Point		getPosition_screen(void)	{ return this->position; }
		void 			setPosition(int x, int y)	{ this->positionTile.x = x; this->positionTile.y = y; this->position.x = x*Printable::tilesize; this->position.y = y*Printable::tilesize ;}
		Direction		isMoving(void)				{ return this->moving; }
		bool			canMove(void)				{ return (this->moving == Direction::NONE); }
		bool			canReach(Entity *entity);

		Waifu			*getWaifu(int idx = 0);
		bool			addWaifu(Waifu_params params);
		bool			addWaifu(Waifu waifu);
};


class NPC : public Entity
{
	private:
		int dialogs[NPC_MAX_DIALOG];
		int nb_dialog = 0;

		void load(NPC_params *params);

	public:
		NPC(NPC_params params);
		NPC(const char *entityName, const char *texturePath);
		~NPC();

		static void	proc_print(SDL_Point offset);
		
		int			i_npc = -1;
		static NPC	*allNPC[MAX_NPC];
		static NPC	*getNPC(SDL_Point pos);

		void		addDialog(int dialog);
		bool		hasDialog(int hist);
		const string  rdmDialogs[3] = {"Bonjour", "Enchanter monsieur", "Il fait beau dehord vous ne trouvez pas ?"};

		static void	  load_history(void);
		static string history[MAX_DIALOG];

		static void load_all(string mapname);
		static void unload_all(void);
};


class Player : public Entity
{
	private:
		unsigned int	i_dglChar;

		NPC				*dialogTarget;
		int 			history;
		bool			inDialog;
		string			dialogText;
		
	public:
		Player(const char *entityName, const char *texturePath);
		~Player();

		virtual void proc(int *ptr_map);
		void 		 print_onMap(SDL_Point offset);

};


class Text : public Printable
{
	private:
		string		text;
		Font_type	type;

		static TTF_Font *fonts[Font_type::LAST_FONT];

	public:
		Text(const char *content, Font_type fontype, SDL_Point pos = {0, 0});
		~Text();
		
		void 		print_onMap(SDL_Point offset = {0, 0});

		void 		setPosition(int x, int y)	{ this->position.x = x; this->position.y = y; }
		void 		setPosition(SDL_Point pos) { this->position = pos; }

		string 		getText(void) { return (text); }

		static void load_font(void);
		static void unload_font(void);
};


class Button : public Printable
{
	private:
		Text 	*text;

		bool 	clicked;
		bool 	hovered;

		SDL_Rect  hitboxMap;

		Button_type type;
		SDL_Color   colorNormal;
		SDL_Color   colorHovered;
		SDL_Color   colorClicked;

		void run_onClick(void) { if (this->onClick_func != NULL) this->onClick_func(this); }

	public:
		Button(SDL_Rect hitbox, Button_type type, string content = "");
		~Button();

		void	(*onClick_func)(Button *obj);	/* void onClick(Button *obj) */
		
		bool	pressed(void)					  { return (this->clicked); }

		void	setColor_Normal(SDL_Color color)  { this->colorNormal  = color; }
		void	setColor_Hovered(SDL_Color color) { this->colorHovered = color; }
		void	setColor_Clicked(SDL_Color color) { this->colorClicked = color; }

		bool	fixed;	/* is sticked to the map */
		int		margin;
		bool	enable;
		void	print_onMap(SDL_Point offset = {0, 0});

		int				i_btn = -1;
		static Button	*allButton[MAX_BUTTON];
		static void		procAll(void);
		static void		printAll(SDL_Point offset);

};



#endif /* _printable_hpp_ */
