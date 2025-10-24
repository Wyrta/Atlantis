#include "Engine.hpp"
#include <filesystem>
namespace fs = std::filesystem;


RenderEngine::RenderEngine(SDL_Renderer* renderer) {
    this->setRenderer(renderer);
}


RenderableItem* RenderEngine::getItem(int id)
{
    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
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
    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if ((*it)->isDisabled() == false)
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

            RenderableItem::textures.push_back((Texture){texture, name, size});
            
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

bool doSendEvent(RenderableItem* item, SDL_FPoint mouse) {
    if (item->isDisabled() == true)
        return false;

    SDL_FRect itemArea = item->getArea();

    if (SDL_PointInRectFloat((const SDL_FPoint*)&mouse, (const SDL_FRect*)&itemArea) == false) 
        return false;

    return true;
}

void RenderEngine::mouseMotion(void) {
    SDL_FPoint mouse;
    SDL_FRect itemArea;

    SDL_GetMouseState(&mouse.x, &mouse.y);

    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if (doSendEvent(*it, mouse) == false)
            continue;

        (*it)->onHover(mouse);
    }
}

void RenderEngine::mouseDown(SDL_MouseButtonEvent event) {
    SDL_FPoint mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if (doSendEvent(*it, mouse) == false)
            continue;

        (*it)->onMouseDown(event);
    }
}

void RenderEngine::mouseUp(SDL_MouseButtonEvent event) {
    SDL_FPoint mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if (doSendEvent(*it, mouse) == false)
            continue;

        (*it)->onMouseUp(event);
    }
}

GameEngine::GameEngine() {
    SDL_Log("test");
}


void GameEngine::process(void) {
    Uint64 ticks = SDL_GetTicks();
    for(int i = 0; i < this->items.size();i += 1)
    {
        this->items[i]->process(ticks);
    }
}

bool newItem(RenderableItem* item) {
    static Uint32 eventType = SDL_RegisterEvents(1);

    if (eventType != 0) {
        SDL_Event event;
        SDL_zero(event);
        event.type = eventType;
        event.user.code = EVENT_NEW_ITEM_REQUEST;
        event.user.data1 = (void *)item;
        event.user.data2 = NULL;
        bool res = SDL_PushEvent(&event);

        if (res == false)
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());

        return true;
    }
    return false;

}