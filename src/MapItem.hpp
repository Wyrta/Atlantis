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
#include <mutex>
#include "RenderableItem.hpp"
#include "GameItem.hpp"
#include "EventHandler.hpp"

typedef enum TileType {
    test = 0,

    empty   // last item
} TileType;

TileType getTiletypeFromStr(std::string type);
std::string getTileTypeName(TileType type);

typedef enum MapEventType {

} MapEventType;

typedef struct MapEvent {
    MapEventType type;
    int src;
    int dst;

    // event specific data

} MapEvent;

class Map;

class Tile : public GameItem {
private:
protected:
    SDL_Point coords;
    SDL_Rect size;   
    TileType type;
    int layer;
    
    static std::vector<MapEvent> messages;
    static std::mutex mutex;
public:
    Tile(TileType type, Map* map, SDL_Point position = {0,0}, SDL_Rect size = {1,1}, int layer = 0);
    ~Tile();

    void setCoords(SDL_Point coords);
    SDL_Point getCoords(void);
    SDL_Rect getSize(void);

    std::vector<MapEvent> getEvent(void);
    void addEvent(MapEvent event);
    void refreshPosition(SDL_FPoint mapPosition);
    
    static std::vector<std::string> tileType;
    static std::string getTileType(std::string type);
    static void loadTileType(void);
    static SDL_FRect tileSize;
};

class Map : public GameItem {
private:
    std::mutex mutex;
    std::vector<Tile*> tilemap;
protected:
public:
    Map();
    ~Map();

    void process(Uint64 ticks);
    void handleEvent(void);

    void addItem(Tile* tile);
    Tile* getItem(int id);
    Tile* getItem(SDL_FPoint position);
    void removeTile(SDL_Point tilePos);

    SDL_FRect getTileSize(void);
    void setTileSize(SDL_FRect size);
};

#endif // MAP_ITEMS