#include "RenderableItem.hpp"
#include <unistd.h>

#define ASSERT_RENDERER     \
    if (renderer == NULL) { \
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error renderer == NULL");\
        return;\
    }


uint32_t RenderableItem::nbId = 0;
std::vector<Texture> RenderableItem::textures;
std::vector<Font> TextSprite::fonts;

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

    if (surface == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Create texte surface : surface NULL -> %s", SDL_GetError());
        return NULL;
    }

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

RenderableItem::RenderableItem(SDL_FPoint pos, EventHandler* eventHandler): id(RenderableItem::nbId++) {
    // SDL_Log("New RenderableItem with id %d", this->id);
    this->area.x = pos.x;
    this->area.y = pos.y;
    
    this->setEventHandler(eventHandler);

    this->canDelete = false;

    this->level = 0;
}

SDL_Texture* RenderableItem::getTexture(std::string name, SDL_FRect* size) {
    for(std::vector<Texture>::iterator it = RenderableItem::textures.begin(); it != RenderableItem::textures.end(); ++it)
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

void RenderableItem::setPosition(SDL_FPoint newPos) {
    this->area.x = newPos.x;
    this->area.y = newPos.y;
}

SDL_FPoint RenderableItem::calculateReelPosition(SDL_FPoint position) {
    SDL_FPoint reelPosition;
    reelPosition.x = this->area.x - position.x;
    reelPosition.y = this->area.y - position.y;

    return reelPosition;
}

void RenderableItem::onHover(SDL_FPoint position, SDL_MouseButtonFlags flags) {
    position = this->calculateReelPosition(position);
    Event event;
    event.type = EventType::onHover;
    event.mousePos = position;
    event.mouseFlags = flags;
    this->sendEvent(event);
}

/**********************************************************************************************************************/

void RenderableGroups::render(SDL_Renderer *renderer) {
    ASSERT_RENDERER

    if (autoUpdate) {
        this->area = this->updateArea();
    }
    else
        this->area = this->getArea();

    this->mutex.lock();
    for (int i = 0; i < this->items.size(); i++) {
        SDL_FPoint position;
        position.x = this->offset[i].x + this->area.x;
        position.y = this->offset[i].y + this->area.y;

        this->items[i]->setPosition(position);

        if (this->items[i]->isDisabled() == true) {
            continue;
        }
        this->items[i]->render(renderer);
    }
    this->mutex.unlock();
}

SDL_FRect RenderableGroups::updateArea(void) {
    SDL_FRect src = this->area;
    SDL_FRect dst;
    src.w = 0;
    src.h = 0;
    this->mutex.lock();
    for (int i = 0; i < this->items.size(); i++) {
        SDL_FRect itemArea = this->items[i]->getArea();
        itemArea.x = this->offset[i].x + this->area.x;
        itemArea.y = this->offset[i].y + this->area.y;

        SDL_GetRectUnionFloat((const SDL_FRect*)&src, (const SDL_FRect*)&itemArea, &dst);
        src = dst;
    }
    this->mutex.unlock();

    return src;
}

void RenderableGroups::moveItem(RenderableItem *item, SDL_FPoint newOffset) {
    this->mutex.lock();

    for (int i = 0; i < this->items.size(); i++) {
        if (this->items[i]->id != item->id)
            continue;

        // change offset
        this->offset[i].x = newOffset.x;
        this->offset[i].y = newOffset.y;

        // apply offset to the item
        newOffset.x = this->offset[i].x + this->area.x;
        newOffset.y = this->offset[i].y + this->area.y;
        item->setPosition(newOffset);
    }
    
    this->mutex.unlock();
}

uint32_t RenderableGroups::addItem(RenderableItem *item) {
    this->mutex.lock();
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
    src = item->getArea();
    src.x = 0;
    src.y = 0;
    itemArea.x = 0;
    itemArea.y = 0;
    dst = {0.0, 0.0, 0.0, 0.0};
    SDL_GetRectUnionFloat((const SDL_FRect*)&src, (const SDL_FRect*)&itemArea, &dst);
    this->area.w = dst.w;
    this->area.h = dst.h;

    this->mutex.unlock();

    return item->id;
}

RenderableItem *RenderableGroups::getItem(uint32_t id) {
    this->mutex.lock();
    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != items.end(); ++it) {
        if ((*it)->id == id) {
            this->mutex.unlock();
            return *it;
        }
    }
    this->mutex.unlock();

    return NULL;
}


RenderableGroups::RenderableGroups(SDL_FPoint pos) : RenderableItem(pos) {

}

RenderableGroups::~RenderableGroups() {
    for (int i = 0; i < this->items.size(); i++) {
        delete this->items[i];
    }
}

/**********************************************************************************************************************/


Sprite::Sprite(std::string path, SDL_FPoint pos) : RenderableItem(pos) {
    this->name = path;
    SDL_FRect tmpArea;
    this->texture = RenderableItem::getTexture(this->name, &tmpArea);
    this->area.x = pos.x;
    this->area.y = pos.y;
    this->area.w = tmpArea.w;
    this->area.h = tmpArea.h;
}

void Sprite::render(SDL_Renderer* renderer) {
    ASSERT_RENDERER

    if (this->texture == NULL) {
        // SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Sprite::render texture == NULL");
        return;
    }

    SDL_FRect area = this->area;
	SDL_RenderTexture(renderer, this->texture, NULL, &area);
}

/**********************************************************************************************************************/

AnimatedSprite::AnimatedSprite(std::vector<std::string> names, int frameDuration, SDL_FPoint pos) : RenderableItem(pos) {
    for (int i = 0;i<names.size();i++) {
        Sprite* item = new Sprite(names[i], (SDL_FPoint){this->area.x, this->area.y});
        frames.push_back(item);

    }
    this->area.h = 32;
    this->area.w = 32;

    this->currentFrameID = 0;
    this->frameDuration = frameDuration;
}

AnimatedSprite::~AnimatedSprite() {
    for (int i = 0; i < this->frames.size(); i++) {
        delete this->frames[i];
    }
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

TextSprite::TextSprite(std::string newContent, int fontSize, SDL_Color color, SDL_FPoint pos) : RenderableItem(pos), fontSize(fontSize) {
    this->setFont();
    this->updateText(newContent);
    this->color = color;
}

TextSprite::~TextSprite() {
    SDL_DestroyTexture(this->texture);
}

void TextSprite::updateText(std::string newContent) {
    if (newContent.find('\r') != -1)
        SDL_Log("TODO: Add carriage return to textsprite");


    this->content = newContent;
    this->update = true;
}

std::string TextSprite::getText(void) {
    return content;
}

TTF_Font* TextSprite::getFont(std::string name, int size) {
    for (int i = 0;i < TextSprite::fonts.size(); i += 1) {
        if ((TextSprite::fonts[i].name == name) && (TextSprite::fonts[i].size == size))
            return TextSprite::fonts[i].font;
    }
    
    std::string path = "assets/";
    path += name;
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (font == NULL) {
        SDL_Log("TextSprite::getFont(): failed to load font : %s", SDL_GetError());
        return NULL;
    }

    Font newFont;
    newFont.font = font;
    newFont.name = name;
    newFont.size = size;

    TextSprite::fonts.push_back(newFont);
    SDL_Log("New font loaded : %s %dpx", name.c_str(), size);

    return newFont.font;
}

void TextSprite::render(SDL_Renderer* renderer) {
    ASSERT_RENDERER
    
    if (this->update) {
        if (this->texture != NULL)
	        SDL_DestroyTexture(this->texture);
        
        SDL_FRect area = {0,0,0,0};
        this->texture = write(renderer, &area, this->font, this->content.c_str(), this->color);
        this->area.h = area.h;
        this->area.w = area.w;
        this->update = false;
    }
    SDL_FRect area = this->area;
	SDL_RenderTexture(renderer, this->texture, NULL, &area);
}

void TextSprite::setFont(std::string fontName) {
    this->font = this->getFont(fontName, this->fontSize);

    if (this->font == NULL)
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "font == NULL");
}


/**********************************************************************************************************************/
