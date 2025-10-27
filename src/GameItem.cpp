#include "GameItem.hpp"
#include "Engine.hpp"

uint32_t GameItem::nbId = 0;

GameItem::GameItem(SDL_FPoint position) : id(nbId++) {
    this->position = position;
    this->renderableItem = NULL;

    bool canDelete = false;
}

GameItem::~GameItem() {
    if (this->renderableItem != NULL)
        this->renderableItem->canDelete = true;
}

void GameItem::setPosition(SDL_FPoint position) {
    this->position = position;
}

void GameItem::setRenderableItem(RenderableItem* renderableItem) {
    if (this->renderableItem != NULL)
        this->renderableItem->canDelete = true;    

    this->renderableItem = renderableItem;
    newItem(this->renderableItem);
}

RenderableItem* GameItem::getRenderableItem(void) {
    return this->renderableItem;
}


/**********************************************************************************************************************/

Popup::Popup(std::string title, std::string content, SDL_FPoint pos) : GameItem(pos) {
    this->currentDuration = 0;
    this->duration = 0;

    this->doRender = false;

    this->title = content;
    this->content = content;

    this->startTicks = 0;

    RenderableGroups* item = new RenderableGroups(this->position);

    item->addItem(new TextSprite(title, "Inter-VariableFont.ttf", 24, {255,255,255,SDL_ALPHA_OPAQUE}));
    item->addItem(new TextSprite(content, "Inter-VariableFont.ttf", 16, {255,255,255,SDL_ALPHA_OPAQUE}, {0.0, 26.0}));
    item->disable();

    this->setRenderableItem(item);
}

void Popup::process(Uint64 ticks) {
    if (this->doRender == false)
        return;

    if (this->startTicks == 0)
        this->startTicks = ticks;

    this->currentDuration = ticks - this->startTicks;

    if ((this->currentDuration > this->duration) && (this->duration != 0)) {
        this->canDelete = true;
        this->renderableItem->disable();
    }
}


void Popup::show(void) {
    this->doRender = true;
    this->renderableItem->enable();

    // reset
    this->currentDuration = 0;
    this->startTicks = 0;
}

void Popup::setcallback(GameItem* callback) {
    this->callback = callback;
}

void Popup::setDuration(int duration) {
    this->duration = duration;
}

int Popup::getDuration(void) {
    return this->duration;
}

/**********************************************************************************************************************/

Text::Text(std::string content, SDL_FPoint pos) : GameItem(pos) {
    this->content = content;
    TextSprite* item = new TextSprite(this->content, "Inter-VariableFont.ttf", 16, {255, 255, 255, SDL_ALPHA_OPAQUE}, this->position);
    this->setRenderableItem((RenderableItem*)item);
}

std::string Text::getText(void) {
    TextSprite* item = (TextSprite*)this->renderableItem;
    return item->getText();
}

void Text::setText(std::string content) {
    TextSprite* item = (TextSprite*)this->renderableItem;
    item->updateText(content);
}

/**********************************************************************************************************************/

TextArea::TextArea(std::string content, SDL_FPoint pos, std::string cursorContent) : Text(content, pos) {
    this->cursor = new TextSprite("_", "Inter-VariableFont.ttf", 16, {255, 255, 255, SDL_ALPHA_OPAQUE}, this->position);
    this->cursor->disable();

    newItem(this->cursor);

    this->lastTicks = 0;
    this->currentDuration = 0;
}

void TextArea::keyPressed(SDL_Scancode key) {
    
}

void TextArea::process(Uint64 ticks) {
    int cursorDuration = 1000;
    this->currentDuration = ticks - this->lastTicks;

    if (this->currentDuration > cursorDuration) {
        this->cursor->toggle();
        this->lastTicks = this->currentDuration;
    }
}