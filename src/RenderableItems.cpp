#include "RenderableItems.hpp"
#include <unistd.h>

#define ASSERT_RENDERER     \
    if (renderer == NULL) { \
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error renderer == NULL");\
        return;\
    }


uint32_t RenderableItems::nbId = 0;
std::vector<Texture> RenderableItems::textures;

SDL_Texture *createTexture(SDL_Renderer* render, SDL_FRect* rectangle, const char* path) {
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	if ( access(path, F_OK) == -1 )
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "File do not exist : '%s'", path);
		return (NULL);
	}

	surface = IMG_Load(path);

	if (surface == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Create surface : '%s'", SDL_GetError());
		return (NULL);
	}

	texture = SDL_CreateTextureFromSurface(render, surface);

	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Create texture : '%s'", SDL_GetError());
		return (NULL);
	}

	SDL_DestroySurface(surface);

	if(rectangle != NULL)
		SDL_GetTextureSize(texture, &rectangle->w, &rectangle->h);

	return (texture);
}

SDL_Texture *write(SDL_Renderer* render, SDL_FRect *rect, TTF_Font *font, const char *text, SDL_Color color) {
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	surface = TTF_RenderText_Solid(font, text, 0, color);

	texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_DestroySurface(surface);

	if (texture == NULL) 
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Create texte texture : texture NULL");
	}
	else
	{
		SDL_GetTextureSize(texture, &rect->w, &rect->h);
	}

	return (texture);
}


/**********************************************************************************************************************/

RenderableItems::RenderableItems(SDL_FPoint pos, GameItem* eventHandler): id(RenderableItems::nbId++) {
    SDL_Log("New RenderableItems with id %d", this->id);
    this->area.x = pos.x;
    this->area.y = pos.y;
    
    this->eventHandler = eventHandler;
}


SDL_Texture* RenderableItems::getTexture(std::string name, SDL_FRect* size) {
    for(std::vector<Texture>::iterator it = RenderableItems::textures.begin(); it != RenderableItems::textures.end(); ++it)
    {
        if ((*it).name != name)
            continue;

        if (size != NULL) {
            size->w = (*it).size.w;
            size->h = (*it).size.h;
        }

        return (*it).texture; 
    }
    return NULL;
}

void RenderableItems::move(SDL_FPoint newPos, int duration) {
    SDL_Log("Error RenderableItems::move not implemented");
}

void RenderableItems::setPosition(SDL_FPoint newPos) {
    this->area.x = newPos.x;
    this->area.y = newPos.y;
}

SDL_FPoint RenderableItems::calculateReelPosition(SDL_FPoint position) {
    SDL_FPoint reelPosition;
    reelPosition.x = this->area.x - position.x;
    reelPosition.y = this->area.y - position.y;

    return reelPosition;
}

void RenderableItems::onHover(SDL_FPoint position) {
    position = this->calculateReelPosition(position);

    if (eventHandler == NULL)
        return;
    eventHandler->onHover(position);
}

void RenderableItems::onClick(SDL_FPoint position) {
    position = this->calculateReelPosition(position);
    if (eventHandler == NULL)
        return;
    eventHandler->onClick(position);
}

void RenderableItems::onHold(SDL_FPoint position) {
    position = this->calculateReelPosition(position);
    if (eventHandler == NULL)
        return;
    eventHandler->onHold(position);
}

void RenderableItems::setEventHandler(GameItem* eventHandler) {
    this->eventHandler = eventHandler;
}


/**********************************************************************************************************************/

void RenderableGroups::render(SDL_Renderer *renderer) {
    ASSERT_RENDERER
    SDL_FRect test = this->area;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &test);   // clear render
    
    for (int i = 0; i < this->items.size(); i++) {
        SDL_FPoint position;
        position.x = this->offset[i].x + this->area.x;
        position.y = this->offset[i].y + this->area.y;

        this->items[i]->setPosition(position);
        this->items[i]->render(renderer);
    }
}

void RenderableGroups::addItem(RenderableItems *item) {
    SDL_FPoint position;
    SDL_FRect itemArea;
    itemArea = item->getArea();

    position.x = itemArea.x;
    position.y = itemArea.y;

    this->offset.push_back(position);

    position.x += this->area.x;
    position.y += this->area.y;

    item->setPosition(position);
    this->items.push_back(item);

    // update area
    SDL_FRect src, dst;
    src = this->area;
    dst = {0.0, 0.0, 0.0, 0.0};
    SDL_GetRectUnionFloat((const SDL_FRect*)&src, (const SDL_FRect*)&itemArea, &dst);
    this->area.w = dst.w;
    this->area.h = dst.h;
}


RenderableGroups::RenderableGroups(SDL_FPoint pos) : RenderableItems(pos) {

}


/**********************************************************************************************************************/


Sprite::Sprite(std::string path, SDL_FPoint pos) : RenderableItems(pos) {
    this->name = path;
    SDL_FRect tmpArea;
    this->texture = RenderableItems::getTexture(this->name, &tmpArea);
    this->area.x = pos.x;
    this->area.y = pos.y;
    this->area.w = tmpArea.w;
    this->area.h = tmpArea.h;
}

void Sprite::render(SDL_Renderer* renderer) {
    ASSERT_RENDERER

    if (this->texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error Sprite::render texture == NULL");
        return;
    }

    SDL_FRect area = this->area;
	SDL_RenderTexture(renderer, this->texture, NULL, &area);
}

/**********************************************************************************************************************/

AnimatedSprite::AnimatedSprite(std::vector<std::string> names, int frameDuration, SDL_FPoint pos) : RenderableItems(pos) {
    for (int i = 0;i<names.size();i++) {
        Sprite* item = new Sprite(names[i], (SDL_FPoint){this->area.x, this->area.y});
        frames.push_back(item);

    }
    this->area.h = 32;
    this->area.w = 32;

    this->currentFrameID = 0;
    this->frameDuration = frameDuration;
}

void AnimatedSprite::render(SDL_Renderer* renderer) {
    ASSERT_RENDERER

    this->frames[this->currentFrameID]->render(renderer);

    if ((this->currentFrameTicks - this->lastFrameTicks) >= this->frameDuration) {
        this->currentFrameID++;

        if (this->currentFrameID >= this->frames.size())
            this->currentFrameID = 0;

        this->currentFrameTicks = this->lastFrameTicks= SDL_GetTicks();
    }
    else {
        this->currentFrameTicks = SDL_GetTicks();
    }
}

void AnimatedSprite::changeFramerate(int frameDuration) {
    this->frameDuration = frameDuration;
}


/**********************************************************************************************************************/

TextSprite::TextSprite(std::string newContent, TTF_Font* font, SDL_Color color, SDL_FPoint pos) : RenderableItems(pos) {
    if (font == NULL)
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "font == NULL");

    this->updateText(newContent);
    this->font = font;
    this->color = color;
}

void TextSprite::updateText(std::string newContent) {
    this->content = newContent;
    this->update = true;
}

std::string TextSprite::getText(void) {
    return content;
}


void TextSprite::render(SDL_Renderer* renderer) {
    ASSERT_RENDERER
    
    if (this->update) {
	    SDL_DestroyTexture(this->texture);
        SDL_FRect area = {0,0,0,0};
        this->texture = write(renderer, &area, this->font, this->content.c_str(), this->color);
        this->area = area;
        this->update = false;
    }

    ASSERT_RENDERER

    
    SDL_FRect area = this->area;
	SDL_RenderTexture(renderer, this->texture, NULL, &area);
}

/**********************************************************************************************************************/
