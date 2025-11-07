#include "MapItem.hpp"
#include "Engine.hpp"

std::vector<MapEvent> Tile::messages;
std::mutex Tile::mutex;
SDL_FRect Tile::tileSize = {0.0, 0.0, 64.0, 64.0};

SDL_FPoint computeVector(bool left, bool right, bool up, bool down) {
    float hypo = 3.0;
    SDL_Point direction = {0, 0};
    float angle = 0;
    
    if (down) { // DOWN
        direction.y -= 1;
    }
    if (up) { // UP
        direction.y += 1;
    }
    if (right) { // RIGHT
        direction.x -= 1;
    }
    if (left) { // LEFT
        direction.x += 1;
    }

    if (direction.x == 0 && direction.y == 0)
        return {0.0, 0.0};
    if (direction.x == 1 && direction.y == 0)
        angle = 0;
    if (direction.x == 1 && direction.y == 1)
        angle = SDL_PI_F/4;
    if (direction.x == 0 && direction.y == 1)
        angle = SDL_PI_F/2;
    if (direction.x == -1 && direction.y == 1)
        angle = 3*SDL_PI_F/4;
    if (direction.x == -1 && direction.y == 0)
        angle = SDL_PI_F;
    if (direction.x == -1 && direction.y == -1)
        angle = 5*SDL_PI_F/4;
    if (direction.x == 0 && direction.y == -1)
        angle = 3*SDL_PI_F/2;
    if (direction.x == 1 && direction.y == -1)
        angle = 7*SDL_PI_F/4;
    
    SDL_FPoint coords;

    coords.x = SDL_cos(angle)*hypo;
    coords.y = SDL_sin(angle)*hypo;

    return coords;
}

/**********************************************************************************************************************/

Map::Map() : GameItem() {
    Tile::tileSize = {0.0, 0.0, 32.0, 32.0};
    RenderableGroups* item = new RenderableGroups(this->getPosition());
    item->setEventHandler(this);
    item->enable();
    item->autoUpdate = false;
    this->setRenderableItem(item);
}

Map::~Map() {
    // delete tile
}

void Map::process(Uint64 ticks) {
    this->handleEvent();

    const bool* keyboard = SDL_GetKeyboardState(NULL);
    
    SDL_FPoint newPos = computeVector(keyboard[SDL_SCANCODE_LEFT], keyboard[SDL_SCANCODE_RIGHT], keyboard[SDL_SCANCODE_UP], keyboard[SDL_SCANCODE_DOWN]);
    newPos.x += this->getPosition().x;
    newPos.y += this->getPosition().y;

    this->setPosition(newPos);


    this->mutex.lock();
    Tile* item = NULL;
    RenderableGroups* group = (RenderableGroups*)this->getRenderableItem();
    SDL_FRect newArea = group->getArea();
    newArea.w = newArea.h = 0.0;
    for(std::vector<Tile*>::iterator it = this->tilemap.begin(); it != this->tilemap.end(); ++it)
    {
        item = *it;
        item->refreshPosition(this->getPosition());
        
        Sprite* rItem = (Sprite*)item->getRenderableItem();
        SDL_FRect itemArea = rItem->getArea();
        SDL_FRect tmp = newArea;
        SDL_GetRectUnionFloat((const SDL_FRect*)&tmp, (const SDL_FRect*)&itemArea, &newArea);
        // newArea.w = tmp.w;
        // newArea.h = tmp.h;
        // SDL_Log("x%f y%f w%f h%f    ->    x%f y%f w%f h%f", itemArea.x, itemArea.y, itemArea.w, itemArea.h, newArea.x, newArea.y, newArea.w, newArea.h);
    }
    newArea.x = this->getPosition().x;
    newArea.y = this->getPosition().y;
    group->setArea(newArea);
    // SDL_Log("x%f y%f w%f h%f", newArea.x, newArea.y, newArea.w, newArea.h);


    this->mutex.unlock();
}

void Map::handleEvent(void) {
    this->mutex.lock();
    for(std::vector<Event>::iterator it = this->event.begin(); it != this->event.end(); ++it)
    {
        Event item = *it;

        switch (item.type)
        {
        case EventType::onKeyDown:
        case EventType::onKeyHold: {
        } break;
        default:
            // SDL_Log("Unhandled: '%s'", getEventTypeName(item.type));
            break;
        }

        // remove event
        it = this->event.erase(it);
        it--;   // get previous iterator
    }
    this->mutex.unlock();
}

void Map::addItem(Tile* tile) {
    this->mutex.lock();
    this->tilemap.push_back(tile);
    tile->getRenderableItem()->disable();
    this->mutex.unlock();
}

Tile* Map::getItem(int id) {
    this->mutex.lock();
    for(std::vector<Tile*>::iterator it = this->tilemap.begin(); it != this->tilemap.end(); ++it)
    {
        if ((*it)->id == id) {
            this->mutex.unlock();
            return *it;
        }
    }
    this->mutex.unlock();
    return NULL;
}

Tile* Map::getItem(SDL_FPoint position) {
    return NULL;
}

void Map::removeTile(SDL_Point tilePos) {

}

SDL_FRect Map::getTileSize(void) {
    return Tile::tileSize;
}

void Map::setTileSize(SDL_FRect size) {
    size.x = 0.0;
    size.y = 0.0;
    Tile::tileSize = size;
}


/**********************************************************************************************************************/

SDL_FPoint computeMapPosition(SDL_FPoint mapOffset, SDL_FRect tileSize, SDL_Point coords) {
    SDL_FPoint finalPosition;

    finalPosition.x = (tileSize.w * coords.x) + mapOffset.x;
    finalPosition.y = (tileSize.h * coords.y) + mapOffset.y;

    return finalPosition;
}

Tile::Tile(std::string type, Map* map, SDL_Point coords, SDL_Rect size, int layer) : GameItem(computeMapPosition(map->getPosition(), Tile::tileSize, coords)) {
    this->coords = coords;
    this->size = size;
    this->layer = layer;

    this->setRenderableItem(new Sprite(type, this->getPosition()));
    Sprite* sprite = (Sprite*)this->getRenderableItem();
    SDL_FRect area = sprite->getArea();
    area.w = Tile::tileSize.w;
    area.h = Tile::tileSize.h;
    sprite->setArea(area);
}

Tile::~Tile() {
    RenderableItem *item = this->getRenderableItem();
    if (item != NULL) {
        item->canDelete = true;
    }
}

SDL_Rect Tile::getSize(void) {
    return this->size;
}

void Tile::refreshPosition(SDL_FPoint mapPosition) {
    this->setPosition(computeMapPosition(mapPosition, Tile::tileSize, this->coords) );
    RenderableItem *item = this->getRenderableItem();
    if (item != NULL) {
        item->setPosition(this->getPosition());
    }
}

void Tile::setCoords(SDL_Point position) {
    this->coords = position;
}

SDL_Point Tile::getCoords(void) {
    return this->coords;
}

std::vector<MapEvent> Tile::getEvent(void) {
    std::vector<MapEvent> itemEvent;
    Tile::mutex.lock();
    for(std::vector<MapEvent>::iterator it = Tile::messages.begin(); it != Tile::messages.end(); ++it)
    {
        if ((*it).dst == this->id) {
            itemEvent.push_back(*it);

            it = Tile::messages.erase(it);
            it--;   // get previous iterator
        }
    }

    Tile::mutex.unlock();
    return itemEvent;
}

void Tile::addEvent(MapEvent event) {
    event.src = this->id;   // to be sure
    Tile::mutex.lock();
    Tile::messages.push_back(event);
    Tile::mutex.unlock();
}
