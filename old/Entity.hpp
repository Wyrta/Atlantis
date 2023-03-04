#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"

using namespace std;

#define NB_DIALOGUE_MAX 3

#define IDLE			0
#define WALK			1
#define ATTACK			2
#define DEAD			3

#define MAX_ENTITY		20
#define MAX_NPC			MAX_ENTITY
#define MAX_ITEM_INV	10
#define MAX_OBJECTIVES	16

#define ATK_DIST		150

#define STAT_ATK_MULT   10
#define STAT_HP_MULT	10
#define STAT_SPEED_MULT 10

#define NONE			0
#define TOP				1
#define LEFT			2
#define BOTTOM			3
#define RIGHT			4

class Entity;
class Player;
class Npc;
class Missions;

/**
 * entity stat
 */
typedef struct stat_s {
	int atk;
	int speed;
	int hp;
} stat_t;

/**
 * All tile are set => top, left, bottom, right
 */
typedef struct textureFormat_s {
	SDL_Rect tile;   								/* texture tile size */
	bool isAttackTiles, isWalkTiles, isIdleTiles;	/* If this texture has this animation set */ 
	SDL_Point attackCoo, walkCoo, idleCoo;
	SDL_Rect attack; 								/* attack tiles position */
	SDL_Rect walk;   								/* walk tiles position   */
	SDL_Rect idle;   								/* idle tiles position   */
} textureFormat_t;

void entityProcAll();
//void entityRenderAll(MapManager *mMap);

class Missions {
	public:
		Missions();
		~Missions();
		
		/**
		 * @brief Print when mission passed
		 * 
		 */
		void print(void);

		/**
		 * @brief Check if player's objective and npcs' is the same 
		 * 
		 */
		bool proc(Player *, Npc *, SDL_Rect *);

	private:
		int state;
		const string dialog[6] = {
			"Joueur - \"Bonjour\"",
			"PNJ - \"Enchant�\"",
			"end",
			"Joueur - \"Yo\"",
			"PNJ - \"Salut !\"",
			"end"
		};


		Uint32 buttons; int btn_x, btn_y;
};

class Entity {
	public:
		const int id;
		static Entity *entityTab[MAX_ENTITY];
		
		/**
		 * Constuctor of Entity
		 * @param render the actual render of the screen 
		 * @param position position in X and Y of the entity
		 * @param entityName the entity name, can be check with isTextureLoad()
		*/
		Entity(SDL_Point position, string entityName);
		~Entity();

		/**
		 * @brief process entity event
		 * 
		 */
		virtual void proc(void);

		/**
		 * Print the entity on the screen
		*/
		bool print(SDL_Rect rectMap);

		/**
		 * Follow the targeted point
		 * @param targetPosition the point to follow
		 * @param rectmap the map rect to not leave
		 * @param screen
		 * @param hitboxMap
		*/
		bool follow(SDL_Point targetPosition, SDL_Rect rectMap, SDL_Rect screen, SDL_Surface *hitboxMap);
  
		bool attack(void);

		/**
		 * @brief Get the Closest entity
		 * @return Entity ptr
		 */
		Entity *getClosest(void);

//		void addItem(int itemID);
//		void delItem(int itemID);

		int item_hp_bonus(void);
		int item_atk_bonus(void);
		int item_speed_bonus(void);

		/* get/set */

		string 	  getName(void)			{ return name;		}

		SDL_Rect  getHitbox(void)		{ return hitbox;	}
		const SDL_Rect  *getHitbox_ptr(void)	{ return &hitbox;	}
		SDL_Point getPosition(void)		{ return position;	}

		int 	  getSpeed(void) 		{ return speed;		}
		void 	  setSpeed(int spd) 	{ speed = spd;		}

		int 	  getMax_hp(void)		{ return max_hp;	}
		int 	  getBase_hp(void)		{ return base_hp;	}
		int 	  getBase_atk(void)		{ return base_atk;	}

		int 	  getState(void) 		{ return state;		}
		void 	  setState(int stt) 	{ state = stt;		}

		SDL_Point getAtkCoo(void)		{ return attackCoo; }
		void	  setAtkCoo_x(int x)	{ attackCoo.x = x; }
		void	  setAtkCoo_y(int y)	{ attackCoo.y = y; }

		int		  getFrame(void)		{ return animationFrame; }
		void	  setFrame(int frm)		{ animationFrame = (frm > 0) ? frm : 0; }

	protected:
		static int 		nbEntity;

		/* entity's base info */

		string 			name;
		SDL_Point 		position;
		SDL_Rect  		hitbox;
		const SDL_Point spawn;
		int 			base_hp, max_hp;
		int 			base_atk;
		int 			dead_timer;

		int  			speed = 10;				/* In px/ticks */
		bool 			weMove = false;
		int  			direction = NONE;
		int  			idleDirection = BOTTOM;

		/* Entity texture */
		SDL_Texture 	*texture = NULL;

		SDL_Point 		attackCoo;

		int 			state, previousState;	/* states of the entity : 0 idle, 1 walk, 2 attack */
		int 			animationFrame;

		textureFormat_t mapTexture;

		stat_t 			stat_lvl;
		
		Mix_Chunk 		*footstep;				/* Entity foot step */
		int				previousFootstep = 0;	/* Last footstep */

		/**
		 * @return True if texture load else false
		*/
		bool isTextureLoad(void) { if(this->texture == NULL) return false; else return true; }

		/**
		 * Load the entity's texute and set its rect
		*/
		void loadTexture(const char *entityName);

		Uint8 red, green, blue;
};

class Player : public Entity {
	public:
		Player(SDL_Point position, string entityName, SDL_Rect *screen, int missionId);

		void proc(void);

		void centerCamera(SDL_Rect *rectMap, SDL_Rect screen);

		int camSpeed = 10;		/* In px/tick */

		bool interaction = false;

		int  getObjective(void)		{ return this->objective; }
		void setObjective(int obj)	{ this->objective = obj;  }
		int  getMissionNum(void)	{ return this->objective; }

		bool tryAttack(SDL_Point target);

	private:
		int objective;
		Missions mission;
		SDL_Rect *rectScreen;
};

class Npc : public Entity {
	public:
		static Npc *npcTab[MAX_NPC];

		Npc(SDL_Point position, string entityName, int missionId);

		/**
		 * @brief when player interact with
		 * 
		 * @return yes
		 */
		bool talk(SDL_Rect screen);

		/**
		 * @brief print on screen a little texte, like "great weather isn't it ?"
		 * 
		 * @return yes
		 */
		bool idleTalk();

		/**
		 * @brief if action manage it 
		 */
		void checkAction(SDL_Rect screen, SDL_Rect playerHitbox);

		int *getObjectives() { return this->objectives; }


	private:
		string dialogue[NB_DIALOGUE_MAX];
		int timerIdle = 1000;
		int dialogueSpeed = 3;

		int objectives[MAX_OBJECTIVES];

		void loadDialogue();

};

#endif /* ENTITY_HPP */
