#ifndef MAP_ITEMS
#define MAP_ITEMS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <iterator>
#include <vector>
#include "RenderableItem.hpp"
#include "EventHandler.hpp"

typedef struct TileType {
    std::string type;
    std::string texture;

} TileType;

class Tile : public GameItem {
private:
protected:
    GameItem* map;
    SDL_Point mapPosition;
public:
    Tile(GameItem* map);

    SDL_FPoint getTilePosition();

    static std::vector<std::string> tileType;
    static std::string getTileType(std::string type);
    static void loadTileType(void);
}

class Map : public GameItem {
private:
protected:
    const SDL_FPoint tileSize;
    std::vector<Tile*> tileMap;
public:
    Map();
    ~Map();
    void addTile(Tile* tile);
    void removeTile(SDL_Point tilePos);
}

#endif // MAP_ITEMS