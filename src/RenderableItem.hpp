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

#define MAX_LEVEL   16

#define RED     (SDL_Color){255,   0,   0, SDL_ALPHA_OPAQUE}
#define GREEN   (SDL_Color){0,   255,   0, SDL_ALPHA_OPAQUE}
#define BLUE    (SDL_Color){0,     0, 255, SDL_ALPHA_OPAQUE}
#define WHITE   (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE}
#define BLACK   (SDL_Color){0,     0,   0, SDL_ALPHA_OPAQUE}

#define DEFAULT_FONT    "Inter-VariableFont.ttf"

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
    bool disabled = false;
    int level;

public:
    const uint32_t id;
    static uint32_t nbId;
    
    static std::vector<Texture> textures;
    static SDL_Texture* getTexture(std::string name, SDL_FRect* size = NULL);
    SDL_FPoint calculateReelPosition(SDL_FPoint position);
    
    virtual void render(SDL_Renderer *renderer) = 0;

    void setPosition(SDL_FPoint newPos);
    SDL_FRect getArea(void) {return this->area;};
    void setArea(SDL_FRect area) {this->area = area;};

    RenderableItem(SDL_FPoint pos = {0,0}, EventHandler* eventHandler = NULL);

    void onHover(SDL_FPoint position, SDL_MouseButtonFlags flags);

    bool isDisabled() {return this->disabled;};
    void disable(void) {this->disabled = true;};
    void enable(void) {this->disabled = false;};
    void toggle(void) {this->disabled = !this->disabled;};

    bool canDelete;
    void setLevel(int level) {if ((level < MAX_LEVEL) && (level >= 0)) this->level = level; };
    int getLevel(void) {return this->level;};
};

class RenderableGroups : public RenderableItem {
private:
    std::vector<RenderableItem*> items;
    std::vector<SDL_FPoint> offset;
    std::mutex mutex;
public:
    void render(SDL_Renderer *renderer);
    uint32_t addItem(RenderableItem *item);
    void moveItem(RenderableItem *item, SDL_FPoint newOffset);
    RenderableItem *getItem(uint32_t item);
    void updateItemsPosition(void);

    bool autoUpdate = true;
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
    
    TTF_Font* font = NULL;
    SDL_Texture* texture = NULL;
    SDL_Color color;

public:
    static std::vector<Font> fonts;
    static TTF_Font* getFont(std::string fontName, int fontSize);

    TextSprite(std::string newContent, int fontSize, SDL_Color color = BLACK, SDL_FPoint pos = {0,0});
    ~TextSprite();
    void updateText(std::string newContent);
    std::string getText(void);
    void setFont(std::string font = DEFAULT_FONT);
    void setColor(SDL_Color color) {this->color = color; this->update = true;};

    void render(SDL_Renderer *renderer);

    const int fontSize;
};

#endif // RENDERABLE_ITEMS