#include "Entity.hpp"

Entity::Entity(SDL_Renderer *render, SDL_Point position, const char *entityName) {

    //Set position of the entity
    mPosition.x = position.x; mPosition.y = position.y;

    //Load the entity's texture
    this->loadTexture(render, entityName);

    //Set hitbox x and y 
    hitbox.x = position.x - hitbox.w/2; 
    hitbox.y = position.y - hitbox.h/2; 
    hitbox.w = 100;
    hitbox.h = 100;

    footstep = Mix_LoadWAV("./sound/noise/walk1.ogg");

}
Entity::~Entity()  { 
    SDL_DestroyTexture(texture); 
    Mix_FreeChunk(footstep); 
}
void Entity::loadTexture(SDL_Renderer *render, const char *entityName) {
    string basePath = "./img/entity/";
    string name = entityName;
    string extension = ".png";

    this->texture = createTexture(render, &hitbox, (basePath+name+extension).c_str());
}
bool Entity::print(SDL_Renderer *render, bool showHitbox) {
    if(showHitbox) {
        SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
        SDL_RenderFillRect(render, &this->hitbox);
    }
    SDL_RenderCopy(render, this->texture, NULL, &hitbox);
    return true; //error manager here future me ;)
}
bool Entity::follow(SDL_Point targetPosition, SDL_Rect rectMap, SDL_Rect screen, SDL_Surface *hitboxMap) {
    weMove = false;
    direction = NONE;


    //Move entity 
/* Prototype move entity
    //to bottom
    if(targetPosition.y > this->mPosition.y + speed) {
        direction = BOTTOM;
        if( GetPixel(hitboxMap, this->mPosition.x, this->mPosition.y + speed/2) > 0) direction = NONE;
        else this->mPosition.y += speed/2;
    }
    //to top
    if(targetPosition.y < this->mPosition.y - speed) {
        direction = TOP;
        if( GetPixel(hitboxMap, this->mPosition.x, this->mPosition.y - speed/2) > 0) direction = NONE;
        else this->mPosition.y -= speed/2;
    }
    //to right
    if(targetPosition.x > this->mPosition.x + speed) {
        direction = RIGHT;
        if( GetPixel(hitboxMap, this->mPosition.x + speed/2, this->mPosition.y) > 0) direction = NONE;
        else this->mPosition.x += speed/2;
    }
    //to left
    if(targetPosition.x < this->mPosition.x - speed) {
        direction = LEFT;
        if( GetPixel(hitboxMap, this->mPosition.x - speed/2, this->mPosition.y) > 0) direction = NONE;
        else this->mPosition.x -= speed/2;
    }
*/

    //to bottom
    if((targetPosition.y > this->mPosition.y + speed) && (this->mPosition.y + this->hitbox.h/2 + speed < rectMap.h)) {
        direction = BOTTOM;
        if( GetPixel(hitboxMap, this->mPosition.x, this->mPosition.y + speed/2 + hitbox.h/3) > 0) direction = NONE;
        else this->mPosition.y += speed/2;
    }
    //to top
    if((targetPosition.y < this->mPosition.y - speed) && (this->mPosition.y - this->hitbox.h/2 - speed > 0)) {
        direction = TOP;
        if( GetPixel(hitboxMap, this->mPosition.x, this->mPosition.y - speed/2 - hitbox.h/3) > 0) direction = NONE;
        else this->mPosition.y -= speed/2;
    }
    //to right
    if((targetPosition.x > this->mPosition.x + speed) && (this->mPosition.x + this->hitbox.w/2 + speed < rectMap.w)) {
        direction = RIGHT;
        if( GetPixel(hitboxMap, this->mPosition.x + speed/2 + hitbox.w/3, this->mPosition.y) > 0) direction = NONE;
        else this->mPosition.x += speed/2;
    }
    //to left
    if((targetPosition.x < this->mPosition.x - speed) && (this->mPosition.x - this->hitbox.w/2 - speed > 0 )) {
        direction = LEFT;
        if( GetPixel(hitboxMap, this->mPosition.x - speed/2 - hitbox.h/3, this->mPosition.y) > 0) direction = NONE;
        else this->mPosition.x -= speed/2;
    }
    //if we move 
    if(direction != NONE) weMove = true;


    //Update entity hitbox
    this->hitbox.x = this->mPosition.x + rectMap.x - this->hitbox.w/2;
    this->hitbox.y = this->mPosition.y + rectMap.y - this->hitbox.h/2;


    //Play sound if he walk
    if(weMove) {

        int distance;
        int n1 = (-this->mPosition.x) - (rectMap.x - screen.w/2);
        int n2 = (-this->mPosition.y) - (rectMap.y - screen.h/2);
        distance = hypot(n1, n2);

        distance /= 5;
        if(distance > 255) distance = 255;
        Mix_SetDistance(1, distance);
        
        if(previousFootstep == 0) //play sound
            Mix_PlayChannel(1, footstep, 0);

        previousFootstep++;
        if(previousFootstep > 25) previousFootstep = 0;

    }

    return weMove;
}

Player::Player(SDL_Renderer *render, SDL_Point position, const char *entityName) : Entity(render, position, entityName) {

}
void Player::centerCamera(SDL_Rect *rectMap, SDL_Rect screen) {
    if((-this->mPosition.x) > (rectMap->x - screen.w/2) + camSpeed) { rectMap->x += camSpeed/2; }
    if((-this->mPosition.x) < (rectMap->x - screen.w/2) - camSpeed) { rectMap->x -= camSpeed/2; }
 
    if((-this->mPosition.y) > (rectMap->y - screen.h/2) + camSpeed) { rectMap->y += camSpeed/2; }
    if((-this->mPosition.y) < (rectMap->y - screen.h/2) - camSpeed) { rectMap->y -= camSpeed/2; }

}

