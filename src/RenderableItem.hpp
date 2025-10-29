#ifndef RENDERABLE_ITEMS
#define RENDERABLE_ITEMS

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
#include "EventHandler.hpp"

SDL_Texture *createTexture(SDL_Renderer* render, SDL_FRect* rectangle, const char* path);

struct Texture {
    SDL_Texture* texture;
    std::string name;
    SDL_FRect size;
};

struct Font {
    TTF_Font* font;
    std::string name;
    int size;
};

class RenderableItem : public EventEmitter {
private:

protected:
    SDL_FRect area = {0, 0, 10, 10};
    bool disabled;

public:
    const uint32_t id;
    static uint32_t nbId;
    
    static std::vector<Texture> textures;
    static SDL_Texture* getTexture(std::string name, SDL_FRect* size = NULL);
    SDL_FPoint calculateReelPosition(SDL_FPoint position);
    
    virtual void render(SDL_Renderer *renderer) = 0;

    void setPosition(SDL_FPoint newPos);
    SDL_FRect getArea(void) {return this->area;};

    RenderableItem(SDL_FPoint pos = {0,0}, EventHandler* eventHandler = NULL);

    void onHover(SDL_FPoint position);

    bool isDisabled() {return this->disabled;};
    void disable(void) {this->disabled = true;};
    void enable(void) {this->disabled = false;};
    void toggle(void) {this->disabled = !this->disabled;};

    bool canDelete;
};

class RenderableGroups : public RenderableItem {
private:
    std::vector<RenderableItem*> items;
    std::vector<SDL_FPoint> offset;
    std::mutex mutex;
public:
    void render(SDL_Renderer *renderer);
    void addItem(RenderableItem *item);
    void updateItemsPosition(void);

    SDL_FRect updateArea(void);

    RenderableGroups(SDL_FPoint pos = {0,0});
    ~RenderableGroups();
};

class Sprite : public RenderableItem {
private:
    std::string name;
    SDL_Texture* texture;

public:
    void render(SDL_Renderer *renderer);

    Sprite(std::string name, SDL_FPoint pos = {0,0});
};

class AnimatedSprite : public RenderableItem {
private:
    std::vector<RenderableItem*> frames;
    int currentFrameID;
    
    int frameDuration;      // ms
    int currentFrameTicks;  // 
    int lastFrameTicks;     // 
    
public:
    AnimatedSprite(std::vector<std::string> names, int frameDuration, SDL_FPoint pos = {0,0});
    ~AnimatedSprite();
    
    void render(SDL_Renderer *renderer);
    void changeFramerate(int frameDuration);
};

class TextSprite : public RenderableItem {
public:
    std::string content;
    bool update;
    
    TTF_Font* font;
    SDL_Texture* texture;
    SDL_Color color;

public:
    static std::vector<Font> fonts;
    static TTF_Font* getFont(std::string fontName, int fontSize);

    TextSprite(std::string newContent, std::string fontName, int fontSize, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint pos = {0,0});
    ~TextSprite();
    void updateText(std::string newContent);
    std::string getText(void);

    void render(SDL_Renderer *renderer);
};

#endif // RENDERABLE_ITEMS