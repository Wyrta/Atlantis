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

SDL_Texture *createTexture(SDL_Renderer* render, SDL_FRect* rectangle, const char* path);


struct Texture {
    SDL_Texture* texture;
    std::string name;
    SDL_FRect size;
};


class RenderableItems {
public:
    const uint32_t id;
    static uint32_t nbId;
    
    static std::vector<Texture> textures;
    static SDL_Texture* getTexture(std::string name, SDL_FRect* size = NULL);
    
    SDL_FRect area = {0, 0, 10, 10};
    virtual void render(SDL_Renderer *renderer) = 0;

    RenderableItems(SDL_FPoint pos = {0,0});
};

class Sprite : public RenderableItems {
private:
    std::string name;
    SDL_Texture* texture;

public:
    void render(SDL_Renderer *renderer);

    Sprite(std::string name, SDL_FPoint pos = {0,0});
};

class AnimatedSprite : public RenderableItems {
private:
    std::vector<RenderableItems*> frames;
    int currentFrameID;
    
    int frameDuration;      // ms
    int currentFrameTicks;  // 
    int lastFrameTicks;     // 
    
public:
    AnimatedSprite(std::vector<std::string> names, int frameDuration, SDL_FPoint pos = {0,0});
    void render(SDL_Renderer *renderer);
};

class TextSprite : public RenderableItems {
public:
    std::string content;
    bool update;
    
    TTF_Font* font;
    SDL_Texture* texture;
    SDL_Color color;

public:
    TextSprite(std::string newContent, TTF_Font* font, SDL_Color color);
    void updateText(std::string newContent);

    void render(SDL_Renderer *renderer);
};

#endif // RENDERABLE_ITEMS