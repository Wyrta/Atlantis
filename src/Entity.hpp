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

#define IDLE    0
#define WALK    1
#define ATTACK  2
#define DEAD    3

#define MAX_ENTITY      20
#define MAX_NPC         MAX_ENTITY
#define MAX_ITEM_INV    10

#define ATK_DIST    150

#define STAT_ATK_MULT   10
#define STAT_HP_MULT    10
#define STAT_SPEED_MULT 10

class Entity;
class Player;
class Npc;

void entityProc();


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
        void proc(void);

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
  
        /**
         * Follow the targeted point
         * @param targetPosition the point to follow
         * @param rectmap the map rect to not leave
         * @param screen
         * @param hitboxMap
        */
        bool attack(void);

        /**
         * @brief Get the Closest entity
         * @return Entity ptr
         */
        Entity *getClosest(void);

        void addItem(int itemID);
        void delItem(int itemID);


        
        string name;

        int base_hp; int max_hp;
        int item_hp_bonus(void);

        int base_atk;
        int item_atk_bonus(void);

        bool isAttack = false;
        SDL_Point attackCoo;

        //in px per ticks 
        int speed = 10;
        int item_speed_bonus(void);
        
        SDL_Point position;
        SDL_Rect  hitbox;

        /* states of the entity : 0 idle, 1 walk, 2 attack */
        int state; int previousState;
        int animationFrame;

        const SDL_Point spawn;

    private:
        static int nbEntity;

        /* Entity texture */
        SDL_Texture *texture = NULL;

        /**
         * All tile are set => top, left, bottom, right
         */
        typedef struct textureFormat_s {
            SDL_Rect tile;   /* texture tile size */

            bool isAttackTiles, isWalkTiles, isIdleTiles;  /* If this texture has this animation set */ 
            SDL_Point attackCoo, walkCoo, idleCoo;
            SDL_Rect attack; /* attack tiles position */
            SDL_Rect walk;   /* walk tiles position   */
            SDL_Rect idle;   /* idle tiles position   */
        } textureFormat_t;

        textureFormat_t mapTexture;

        /**
         * entity stat
         */
        typedef struct stat_s {
            int atk;
            int speed;
            int hp;
        } stat_t;

        stat_t stat_lvl;

        /* Entity foot step */
        Mix_Chunk *footstep;
        /* Last footstep */
        int previousFootstep = 0;

        /**
         * @return True if texture load else false
        */
        bool isTextureLoad(void) { if(this->texture == NULL) return false; else return true; }

        /**
         * Load the entity's texute and set its rect
        */
        void loadTexture(const char *entityName);


        #define NONE    0
        #define TOP     1
        #define LEFT    2
        #define BOTTOM  3
        #define RIGHT   4

        Uint8 red, green, blue;

        bool weMove = false;
        int direction = NONE;
        int idleDirection = BOTTOM;
        int dead_timer;
};

class Player : public Entity {
    public:
        Player(SDL_Point position, string entityName);
        void centerCamera(SDL_Rect *rectMap, SDL_Rect screen);

        //bool print(bool showHitbox, SDL_Rect rectMap);


        /* in px per ticks */
        int camSpeed = 10;

        bool interaction = false;


};

class Npc : public Entity {
    public:
        static Npc *npcTab[MAX_NPC];

        Npc(SDL_Point position, string entityName);
        /**
         * @brief when player interact with
         * 
         * @return yes
         */
        bool talk(SDL_Window *window, SDL_Rect screen);
        /**
         * @brief print on screen a little texte, like "great weather isn't it ?"
         * 
         * @return yes
         */
        bool idleTalk();
        /**
         * @brief if action manage it 
         */
        void checkAction(SDL_Window *window, SDL_Rect screen, SDL_Rect playerHitbox);

    private:
        string dialogue[NB_DIALOGUE_MAX];
        int timerIdleTable = 1000;
        int dialogueSpeed = 3;


        void loadDialogue();

};

#endif /* ENTITY_HPP */

