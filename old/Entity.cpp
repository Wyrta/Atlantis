#include "Entity.hpp"
#include "console.hpp"
#include "eventManager.hpp"

extern SDL_Renderer	*render;
extern SDL_Window	*window;
extern Console 		*console;
extern EventManager	*eventManager;

int	    Entity::nbEntity = 0;
Entity *Entity::entityTab[MAX_ENTITY];
Npc	   *Npc::npcTab[MAX_NPC];


void entityProcAll()
{
	/* get all entity in the tab */
	for(int i = 0; i < MAX_ENTITY; i++)
	{
		if (Entity::entityTab[i] != NULL)
		{
			Entity::entityTab[i]->proc();
		}
	}
}

//void entityRenderAll(MapManager *mMap)
//{
//	/* get all entity in the tab */
//	for(int i = 0; i < MAX_ENTITY; i++)
//	{
//		if (Entity::entityTab[i] != NULL)
//		{
//			Entity::entityTab[i]->print(mMap->rectMap);
//		}
//	}
//}
	
Entity::Entity(SDL_Point mPosition, string entityName) : id(Entity::nbEntity), spawn({mPosition.x, mPosition.y})
{
	/* Set entity tab and ID */
	Entity::nbEntity++;
	if (id >= MAX_ENTITY)
		console->log("Too many entity");

	this->entityTab[this->id] = this;
	cout << "Entity " << this->id << endl;

	this->speed 			= 10;				
	this->weMove 			= false;
	this->direction 		= NONE;
	this->idleDirection 	= BOTTOM;
	
	this->name.assign(entityName.c_str());

	/* Reset stat */

	this->stat_lvl.hp		= 0;
	this->stat_lvl.atk		= 0;
	this->stat_lvl.speed	= 0;

	/* Set base atk and base hp */
	this->base_atk = 30;
	this->base_hp  = 100;
	this->max_hp   = 100;

	/* Set position of the entity */
	position.x = mPosition.x; position.y = mPosition.y;

	/* Load the entity's texture */
	this->loadTexture(entityName.c_str());
	this->state = IDLE;

	/* Set hitbox x and y */
	hitbox.x = position.x - hitbox.w/2; 
	hitbox.y = position.y - hitbox.h/2; 
	hitbox.w = 100;
	hitbox.h = 100;

	footstep = Mix_LoadWAV("./sound/noise/walk1.ogg");
}

Entity::~Entity()
{

	SDL_DestroyTexture(texture); 
	Mix_FreeChunk(footstep); 

	cout << "Entity " << this->id << " destroyed" << endl; 

}

void Entity::proc(void)
{
	if (this->state == DEAD) 
	{
		if ((int)SDL_GetTicks() > this->dead_timer)
		{
			this->position.x = this->spawn.x;
			this->position.y = this->spawn.y;
			this->state = IDLE;
			console->log("Entity %d respawned", this->id);
		}
		return;
	}

	if (this->base_hp <= 0)
	{
		console->log("Entity %d has died", this->id);


		dead_timer = SDL_GetTicks() + 3000;
		this->state = DEAD;
	
		base_hp = 100;
		return;
	}

	if (this->state == ATTACK)
	{
		console->log("attack proc frame : %d", this->animationFrame);
		if (this->animationFrame == 39)
			this->attack();
	}
}

void Entity::loadTexture(const char *entityName)
{
	string basePath = "./img/entity/";
	string name = entityName;
	string extension = ".png";

	/* read config file here and setup mapTexture data*/
	
	ifstream configFile("./config/entity_texture.config");
	string line, atkStr("attack"), walkStr("walk"), idleStr("idle");
	int lineIndex(1), index;

	while (getline(configFile, line) ) {
		index = line.find(name);
		if(index != (int)string::npos) {
//			cout << "texture found for " << name << this->id << ", at line " << lineIndex << endl;

			/* tile size */

			int leftBracketTile  = index + name.length();
			int rightBracketTile = line.find(";", index);

			this->mapTexture.tile.x = 0;
			this->mapTexture.tile.y = 0;
			this->mapTexture.tile.w = stoi(line.substr(leftBracketTile, rightBracketTile));
			this->mapTexture.tile.h = stoi(line.substr(leftBracketTile, rightBracketTile));

//			cout << "-tile : " << this->mapTexture.tile.w << endl;

			/* Find attack animation */
			index = line.find(atkStr);
			if(index != (int)string::npos) {
				this->mapTexture.isAttackTiles = true;

				int leftBracket  = index + atkStr.length();
				int rightBracket = line.find(";", index);

				string tmp = line.substr(leftBracket, rightBracket);

				this->mapTexture.attack.x = 0;
				this->mapTexture.attack.y = stoi(tmp);
				this->mapTexture.attack.w = this->mapTexture.tile.w;
				this->mapTexture.attack.h = this->mapTexture.tile.h;

				this->mapTexture.attackCoo.x = this->mapTexture.attack.x; 
				this->mapTexture.attackCoo.y = this->mapTexture.attack.y; 


//				cout << "-attack : " << this->mapTexture.attack.y << endl;
			} 
			else this->mapTexture.isAttackTiles = false;

			/* Find walk animation */
			index = line.find(walkStr);
			if(index != (int)string::npos) {
				
				this->mapTexture.isWalkTiles = true;

				int leftBracket  = index + walkStr.length();
				int rightBracket = line.find(";", index);

				string tmp = line.substr(leftBracket, rightBracket);

				this->mapTexture.walk.x = 0;
				this->mapTexture.walk.y = stoi(tmp);
				this->mapTexture.walk.w = this->mapTexture.tile.w;
				this->mapTexture.walk.h = this->mapTexture.tile.h;

				this->mapTexture.walkCoo.x = this->mapTexture.walk.x; 
				this->mapTexture.walkCoo.y = this->mapTexture.walk.y; 
				

//				cout << "-walk : " << this->mapTexture.walk.y << endl;
			} 
			else 
				this->mapTexture.isWalkTiles = false;

			/* Find idle animation */
			index = line.find(idleStr);
			if(index != (int)string::npos)
			{
				this->mapTexture.isIdleTiles = true;

				int leftBracket  = index + idleStr.length();
				int rightBracket = line.find(";", index);

				string tmp = line.substr(leftBracket, rightBracket);

				this->mapTexture.idle.x = 0;
				this->mapTexture.idle.y = stoi(tmp);
				this->mapTexture.idle.w = this->mapTexture.tile.w;
				this->mapTexture.idle.h = this->mapTexture.tile.h;

				this->mapTexture.idleCoo.x = this->mapTexture.idle.x; 
				this->mapTexture.idleCoo.y = this->mapTexture.idle.y; 


//				cout << "-idle : " << this->mapTexture.idle.y << endl;
			} 
			else this->mapTexture.isIdleTiles = false;
		}
		lineIndex++;
	}

	cout
	 << " idle = " << this->mapTexture.isIdleTiles 
	 << " walk = " << this->mapTexture.isWalkTiles
	 << " atk  = " << this->mapTexture.isAttackTiles 
	<< endl;

	configFile.close();


	this->texture = createTexture(render, &hitbox, (basePath+name+extension).c_str());
}

bool Entity::print(SDL_Rect rectMap)
{
	/* Update entity hitbox */
	this->hitbox.x = this->position.x + rectMap.x - this->hitbox.w/2;
	this->hitbox.y = this->position.y + rectMap.y - this->hitbox.h/2;

	/* if the state dont change lets animate it */
	if((previousState == state) && animationFrame <= 40) 
		animationFrame++;
	else 
		animationFrame = 0;

	/* choose the perfect rendered texture for the state of the entity */
	switch (this->state) {
		case IDLE: 
			if(this->mapTexture.isIdleTiles) {
				this->mapTexture.idle.x = (animationFrame/10)*this->mapTexture.idle.w;
				SDL_RenderCopy(render, this->texture, &this->mapTexture.idle, &hitbox);
			} 
			else if(this->mapTexture.isWalkTiles) {
				this->mapTexture.walk.x = 0;
				this->mapTexture.walk.y = this->mapTexture.walkCoo.y + idleDirection*this->mapTexture.tile.w - 50;
				SDL_RenderCopy(render, this->texture, &this->mapTexture.walk, &hitbox);
			}
			else SDL_RenderCopy(render, this->texture, &this->mapTexture.tile, &hitbox);
			break;
		case WALK:
			if(this->mapTexture.isWalkTiles) {
				this->mapTexture.walk.x = (animationFrame/10)*this->mapTexture.walk.w;
				this->mapTexture.walk.y = this->mapTexture.walkCoo.y + direction*this->mapTexture.tile.w - 50;
				SDL_RenderCopy(render, this->texture, &this->mapTexture.walk, &hitbox);
			}
			else SDL_RenderCopy(render, this->texture, &this->mapTexture.tile, &hitbox);
			break;
		case ATTACK:
			if(this->mapTexture.isAttackTiles)  {

				this->mapTexture.attack.x = (animationFrame/10)*this->mapTexture.attack.w;
				this->mapTexture.attack.y = this->mapTexture.attackCoo.y + direction*this->mapTexture.tile.w - 50;
				SDL_RenderCopy(render, this->texture, &this->mapTexture.attack, &hitbox);
			}
			else SDL_RenderCopy(render, this->texture, &this->mapTexture.tile, &hitbox);
		
			animationFrame++;

			if(animationFrame >= 40) 
				this->state = IDLE;

			break;
		case DEAD:
			SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
			SDL_RenderFillRect(render, &this->hitbox);
			break;

		default: 
			SDL_RenderCopy(render, this->texture, &this->mapTexture.tile, &hitbox);
			break;
	}

	previousState = state;
	return true; /* error manager here future me ;) */
}

bool Entity::follow(SDL_Point targetPosition, SDL_Rect rectMap, SDL_Rect screen, SDL_Surface *hitboxMap)
{
	this->weMove = false;
	this->direction = NONE;

	int speed_tot = this->speed + this->item_speed_bonus();

	if (this->state == ATTACK) 
	{
		int posPlayer_x = this->position.x - (this->hitbox.w/2);
		int posPlayer_y = this->position.y - (this->hitbox.h/2);

		int mouseOnPlayer_x = attackCoo.x - posPlayer_x;
		int mouseOnPlayer_y = attackCoo.y - posPlayer_y;

		if(mouseOnPlayer_y > 0 && (mouseOnPlayer_y > mouseOnPlayer_x || mouseOnPlayer_y > -mouseOnPlayer_x)) direction = BOTTOM;
		if(mouseOnPlayer_x > 0 && (mouseOnPlayer_x > mouseOnPlayer_y || mouseOnPlayer_x > -mouseOnPlayer_y)) direction = RIGHT;
		if(mouseOnPlayer_x < 0 && (mouseOnPlayer_x < mouseOnPlayer_y || mouseOnPlayer_x < -mouseOnPlayer_y)) direction = LEFT;
		if(mouseOnPlayer_y < 0 && (mouseOnPlayer_y < mouseOnPlayer_x || mouseOnPlayer_y < -mouseOnPlayer_x)) direction = TOP;

		return true;
	}

	if (this->state == IDLE || this->state == WALK) 
	{
	/* Move entity */
		/* to bottom */
		if((targetPosition.y > this->position.y + speed_tot) && (this->position.y + this->hitbox.h/2 + speed_tot < rectMap.h)) {
			direction = BOTTOM;
			if( GetPixel(hitboxMap, this->position.x, this->position.y + speed_tot/2 + hitbox.h/3) > 0) direction = NONE;
			else this->position.y += speed_tot/2;
		}
		/* to top */ 
		if((targetPosition.y < this->position.y - speed_tot) && (this->position.y - this->hitbox.h/2 - speed_tot > 0)) {
			direction = TOP;
			if( GetPixel(hitboxMap, this->position.x, this->position.y - speed_tot/2 - hitbox.h/3) > 0) direction = NONE;
			else this->position.y -= speed_tot/2;
		}
		/* to right */
		if((targetPosition.x > this->position.x + speed_tot) && (this->position.x + this->hitbox.w/2 + speed_tot < rectMap.w)) {
			direction = RIGHT;
			if( GetPixel(hitboxMap, this->position.x + speed_tot/2 + hitbox.w/3, this->position.y) > 0) direction = NONE;
			else this->position.x += speed_tot/2;
		}
		/* to left */
		if((targetPosition.x < this->position.x - speed_tot) && (this->position.x - this->hitbox.w/2 - speed_tot > 0 )) {
			direction = LEFT;
			if( GetPixel(hitboxMap, this->position.x - speed_tot/2 - hitbox.h/3, this->position.y) > 0) direction = NONE;
			else this->position.x -= speed_tot/2;
		}
	
		/* if we move */
		if(direction != NONE) {
			weMove = true;
			this->state = WALK;
			idleDirection = direction;
		}
		else if (direction == NONE)
		{
			this->state = IDLE;
		}
	}



	/* Update entity hitbox */
	this->hitbox.x = this->position.x + rectMap.x - this->hitbox.w/2;
	this->hitbox.y = this->position.y + rectMap.y - this->hitbox.h/2;


	/* Play sound if he walk */
	if(weMove) {

		int distance;
		int n1 = (-this->position.x) - (rectMap.x - screen.w/2);
		int n2 = (-this->position.y) - (rectMap.y - screen.h/2);
		distance = hypot(n1, n2);

		distance /= 5;
		if(distance > 255) distance = 255;
		Mix_SetDistance(1, distance);
		
		if(previousFootstep == 0) /*play sound */
			Mix_PlayChannel(1, footstep, 0);

		previousFootstep++;
		if(previousFootstep > 25) previousFootstep = 0;

	}

	return weMove;
}

bool Entity::attack(void)
{
	Entity *target = this->getClosest();

	if (target == NULL)
		return false;

	int n1 = std::abs(this->entityTab[target->id]->position.x - this->position.x);
	int n2 = std::abs(this->entityTab[target->id]->position.y - this->position.y);
/*
	if (n1 < 0)
		n1 = -n1;
	if (n2 < 0)
		n2 = -n2;
*/
	int result = hypot(n1, n2);

	console->log("Attack dist result %d", result);

	if (result < ATK_DIST)
		target->base_hp = target->base_hp - (this->base_atk + this->item_atk_bonus());

	return true;
}

Entity *Entity::getClosest(void)
{
	int closest_id   = 0;
	int closest_dist = INT32_MAX;
	
	for(int i = 0; i < MAX_ENTITY; i++)
	{
		if (this->entityTab[i] != NULL && this->entityTab[i]->id != this->id)
		{
			int n1 = this->entityTab[i]->position.x - this->position.x;
			int n2 = this->entityTab[i]->position.y - this->position.y;

			if (n1 <= 0)
				n1 = -n1;
			if (n2 <= 0)
				n2 = -n2;

			int result = hypot(n1, n2);

//			cout << "distance between " << this->entityTab[i]->id << " and " << this->id << " is : " << result << endl;

			if (result < closest_dist)
			{
				closest_id = this->entityTab[i]->id;
				closest_dist = result;
			}
		}
	}

//	cout << "Entity " << closest_id << " is the closest (" << closest_dist << "px)" << endl;
	console->log("Entity fund : %s", this->entityTab[closest_id]->name);
	return this->entityTab[closest_id];
}

int Entity::item_atk_bonus(void)
{
	int bonus = this->stat_lvl.atk * STAT_ATK_MULT;
 
	return bonus; 
}

int Entity::item_hp_bonus(void)
{
	int bonus = this->stat_lvl.hp * STAT_HP_MULT;

	return bonus; 
}

int Entity::item_speed_bonus(void)
{
	int bonus = this->stat_lvl.speed* STAT_SPEED_MULT;

	return bonus; 
}


Player::Player(SDL_Point position, string entityName, SDL_Rect *screen, int missionId) : Entity(position, entityName)
{
	rectScreen 	= screen;
	objective	= missionId;
}

void Player::centerCamera(SDL_Rect *rectMap, SDL_Rect screen)
{
	rectMap->x = -this->position.x + screen.w/2;
	rectMap->y = -this->position.y + screen.h/2;
/*
	if((-this->position.x) > (rectMap->x - screen.w/2) + this->camSpeed) { rectMap->x += this->camSpeed/2; }
	if((-this->position.x) < (rectMap->x - screen.w/2) - this->camSpeed) { rectMap->x -= this->camSpeed/2; }
 
	if((-this->position.y) > (rectMap->y - screen.h/2) + this->camSpeed) { rectMap->y += this->camSpeed/2; }
	if((-this->position.y) < (rectMap->y - screen.h/2) - this->camSpeed) { rectMap->y -= this->camSpeed/2; }

*/
}

void Player::proc(void)
{
	/* if player interact */
	if(this->interaction)
	{
		/* test with entity */
		for(int i = 0; i < MAX_NPC; i++)
		{
			if (Npc::npcTab[i] != NULL)
			{
				if (collision(this->getHitbox(), Npc::npcTab[i]->getHitbox()) )
				{
					/* proc dialogs */
					if (mission.proc(this, Npc::npcTab[i], this->rectScreen) )
						break;
					if (Npc::npcTab[i]->talk(*this->rectScreen) )
						break;
				}
			}
		} 
		
		/* test with idk what */
		
		/* ... */
		

		/* end of proc we can reset flag */
		this->interaction = false; 
	}

	/* if player's dead */
	if (this->state == DEAD) 
	{
		if ((int)SDL_GetTicks() > this->dead_timer)
		{
			this->position.x = this->spawn.x;
			this->position.y = this->spawn.y;
			this->state = IDLE;
			console->log("Entity %d respawned", this->id);
		}
		return;
	}

	/*  */
	if (this->base_hp <= 0)
	{
		console->log("Entity %d has died", this->id);


		dead_timer = SDL_GetTicks() + 3000;
		this->state = DEAD;
	
		base_hp = 100;
		return;
	}

	if (this->state == ATTACK)
	{
		console->log("attack proc frame : %d", this->animationFrame);
		if (this->animationFrame == 39)
			this->attack();
	}


}


bool Player::tryAttack(SDL_Point target)
{
	if (state != ATTACK)
	{
		state		= ATTACK;

		attackCoo.x = target.x;
		attackCoo.y = target.y;

		return true;
	}
	else
	{
		return false;
	}
}


Npc::Npc(SDL_Point position, string entityName, int missionId) : Entity(position, entityName)
{	
	this->npcTab[this->id] = this;

	for (int i = 0; i < MAX_OBJECTIVES; i++)
		objectives[i] = -1;

	this->objectives[0] = missionId;
	this->objectives[10] = 3;
	
	loadDialogue();
}

void Npc::loadDialogue()
{
	dialogue[0] = "Bonjour messire.";
	dialogue[1] = "Enchante monsieur.";
	dialogue[2] = "Belle journee n'est-il pas ?";
}

void Npc::checkAction(SDL_Rect screen, SDL_Rect playerHitbox)
{
	if(collision(playerHitbox, this->hitbox) )
	{
		this->talk(screen);
	} else return;
}

bool Npc::idleTalk()
{
	if(timerIdle <= 0)
	{
		console->log("Entity %d : idle", this->id);

		timerIdle = 1000;
		return true;
	}
	timerIdle--;

	return false;
}

bool Npc::talk(SDL_Rect screen)
{
	string mDialogue = dialogue[intAleat(0,NB_DIALOGUE_MAX)];

	printDialog(render, screen, mDialogue, "talking", dialogueSpeed);
	eventManager->waitForMouse(SDL_BUTTON_LMASK, 0);

	return true;
}


Missions::Missions()
{
	state = 0;  // read save file
}

Missions::~Missions()
{

}

void Missions::print(void)
{

}

bool Missions::proc(Player *player, Npc *npc, SDL_Rect *screen)
{
	int playerObj = player->getObjective();
	int *npcObj   = npc->getObjectives();
	int npcMissionId = -1; 

	/* check missions */
	for (int i = 0; i < MAX_OBJECTIVES; i++)
	{
		if (playerObj == npcObj[i])
		{
			npcMissionId = i;
			break;
		}
	}
	/* if no mission available => quit */
	if (npcMissionId == -1)
		return false;

	do {
		/* print dialogue */
		printDialog(render, *screen, this->dialog[this->state], "talking", 3);
		eventManager->waitForMouse(SDL_BUTTON_LMASK, 0);
	} while (this->dialog[++this->state].compare("end") != 0);
	
	player->setObjective(state);
	return true;
}