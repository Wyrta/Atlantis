#include "Engine.hpp"
#include <filesystem>
namespace fs = std::filesystem;


RenderEngine::RenderEngine(SDL_Renderer* renderer) {
    this->setRenderer(renderer);
}


RenderableItems* RenderEngine::getItem(int id)
{
    for(std::vector<RenderableItems*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if ((*it)->id == id)
            return *it;
    }

    return NULL;
}

void RenderEngine::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
}

void RenderEngine::render(void) {
    for(std::vector<RenderableItems*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        (*it)->render(this->renderer);
    }
}

int RenderEngine::loadTextures(void) {
    std::string path = "assets";
    int nbTexture = 0;

    for (const auto & entry : fs::directory_iterator(path)) {
        std::string filename = entry.path().string();
        
        if (filename.ends_with(".png")) {
            SDL_Log("File found: %s", filename.c_str());
            
            SDL_FRect size;
            SDL_Texture* texture = createTexture(this->renderer, &size, filename.c_str());
            
            std::string name = filename.substr(filename.find("/")+1);
            SDL_Log("Texture name: %s", name.c_str());

            RenderableItems::textures.push_back((Texture){texture, name, size});
            
            nbTexture++;
        }
    }

    SDL_Log("Loaded %d textures", nbTexture);

    return nbTexture;
}

void RenderEngine::clearScreen(void) {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, NULL);   // clear render
}

void RenderEngine::mouseMotion(void) {
    SDL_FPoint mouse;
    SDL_FRect itemArea;

    for(std::vector<RenderableItems*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        itemArea = (*it)->getArea();

        if (SDL_PointInRectFloat((const SDL_FPoint*)&mouse, (const SDL_FRect*)&itemArea) == false) 
            continue;

        (*it)->onHover(mouse);
    }
}

void RenderEngine::mouseClick(void) {
    SDL_FPoint mouse;
    SDL_FRect itemArea;
    Uint32 mask = SDL_GetMouseState(&mouse.x, &mouse.y);
    bool left  = (mask & SDL_BUTTON_LMASK);
    // bool right = (mask & SDL_BUTTON_RMASK);
    // bool middle = (mask & SDL_BUTTON_MMASK);
    // bool x1 = (mask & SDL_BUTTON_X1MASK);
    // bool x2 = (mask & SDL_BUTTON_X2MASK);

    for(std::vector<RenderableItems*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        itemArea = (*it)->getArea();

        if (SDL_PointInRectFloat((const SDL_FPoint*)&mouse, (const SDL_FRect*)&itemArea) == false) 
            continue;

        if (left == true) {
            SDL_Log("Click on %d", (*it)->id);
            (*it)->onClick(mouse);
        }
    }
}
