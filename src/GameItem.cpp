#include "GameItem.hpp"

uint32_t GameItem::nbId = 0;

GameItem::GameItem(SDL_FPoint position) : id(nbId++) {
    this->position = position;
    this->renderableItem = NULL;

}

void GameItem::setPosition(SDL_FPoint position) {
    this->position = position;
}

void GameItem::setRenderableItem(RenderableItem* renderableItem) {
    this->renderableItem = renderableItem;
}

void GameItem::onHover(SDL_FPoint position) {

}

void GameItem::onClick(SDL_FPoint position) {

}

void GameItem::onHold(SDL_FPoint position) {

}

