#include "RenderableItems.hpp"
#include <unistd.h>


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

RenderableItems::RenderableItems(SDL_FPoint pos): id(RenderableItems::nbId++) {
    SDL_Log("New RenderableItems with id %d", this->id);
    this->area.x = pos.x;
    this->area.y = pos.y;
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
/**********************************************************************************************************************/

Sprite::Sprite(std::string path, SDL_FPoint pos) : RenderableItems(pos) {
    this->name = path;

    this->texture = RenderableItems::getTexture(this->name, &this->area);
    // SDL_Log("0x%x", this->texture);
}

void Sprite::render(SDL_Renderer* renderer) {
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error Sprite::render renderer == NULL");
        return;
    }

    if (this->texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error Sprite::render texture == NULL");
        return;
    }

	SDL_RenderTexture(renderer, this->texture, NULL, &this->area);
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

/**********************************************************************************************************************/

TextSprite::TextSprite(std::string newContent, TTF_Font* font, SDL_Color color) {
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

void TextSprite::render(SDL_Renderer* renderer) {
    if (this->update) {
	SDL_DestroyTexture(this->texture);
        this->texture = write(renderer, &this->area, this->font, this->content.c_str(), this->color);
        this->update = false;
    }

	SDL_RenderTexture(renderer, this->texture, NULL, &this->area);
}

/**********************************************************************************************************************/
