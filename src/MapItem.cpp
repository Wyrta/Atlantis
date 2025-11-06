#include "MapItem.hpp"
#include "Engine.hpp"

std::vector<MapEvent> Tile::messages;
std::mutex Tile::mutex;
SDL_FRect Tile::tileSize = {0.0, 0.0, 64.0, 64.0};

/**********************************************************************************************************************/

Map::Map() : GameItem() {
    Tile::tileSize = {0.0, 0.0, 100.0, 100.0};
    this->setRenderableItem(new RenderableGroups(this->getPosition()));
    this->getRenderableItem()->setEventHandler(this);
}

Map::~Map() {
    // delete tile
}

void Map::process(Uint64 ticks) {
    this->handleEvent();

    this->mutex.lock();
    Tile* item = NULL;
    for(std::vector<Tile*>::iterator it = this->tilemap.begin(); it != this->tilemap.end(); ++it)
    {
        item = *it;
        item->refreshPosition(this->getPosition());
    }
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
            SDL_FPoint newPos = this->getPosition();

            switch (item.scancode)
            {
            case SDL_Scancode::SDL_SCANCODE_UP: newPos.y -= 5; this->setPosition(newPos); break;  
            case SDL_Scancode::SDL_SCANCODE_DOWN: newPos.y += 5; this->setPosition(newPos); break;  
            case SDL_Scancode::SDL_SCANCODE_LEFT: newPos.x -= 5; this->setPosition(newPos); break;  
            case SDL_Scancode::SDL_SCANCODE_RIGHT: newPos.x += 5; this->setPosition(newPos); break;  
            
            default:
                SDL_Log("unknow: '%d' '%c'", item.key, item.key);
                break;
            }
        }
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
