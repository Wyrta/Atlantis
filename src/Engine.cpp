#include "Engine.hpp"
#include <filesystem>
namespace fs = std::filesystem;


RenderEngine::RenderEngine(SDL_Renderer* renderer) {
    this->setRenderer(renderer);
    this->keyboardTarget = -1;
}

int RenderEngine::addItem(RenderableItem* item) {
    this->mutex.lock();

    this->items.push_back(item);
    int size = this->items.size();
    
    this->mutex.unlock();

    return size;
}


RenderableItem* RenderEngine::getItem(int id) {
    this->mutex.lock();
    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if ((*it)->id == id) {
            this->mutex.unlock();
            return *it;
        }
    }

    this->mutex.unlock();
    return NULL;
}

void RenderEngine::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
}

void RenderEngine::render(void) {
    RenderableItem* item = NULL;
    this->mutex.lock();
    for(std::vector<RenderableItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        item = *it;
        if (item->isDisabled() == false)
            item->render(this->renderer);

        if (item->canDelete == true) {
            it = this->items.erase(it);
            it--;   // get previous iterator

            delete item;
        }
    }
    this->mutex.unlock();
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

void RenderEngine::requestKeybordTarget(int id) {
    SDL_Log("requestKeybordTarget %d", id);
    this->keyboardTarget = id;
}

void RenderEngine::onText(SDL_TextInputEvent event) {
    if (this->keyboardTarget < 0)
        return;

    EventEmitter* item = (EventEmitter*)this->getItem(this->keyboardTarget);

    if (item == NULL)
        return;

    item->onTextInput(event.text);
    
}

void RenderEngine::onKey(SDL_KeyboardEvent event) {
    if (this->keyboardTarget < 0)
        return;

    EventEmitter* item = (EventEmitter*)this->getItem(this->keyboardTarget);

    if (item == NULL)
        return;

    SDL_Keycode key = SDL_GetKeyFromScancode(event.scancode, event.mod, false);

    if (event.down) {
        if (event.repeat) 
            item->onKeyHold(key);
        else
            item->onKeyDown(key);
    }
    else
        item->onKeyUp(key);
}


GameEngine::GameEngine() {

}


void GameEngine::process(void) {
    Uint64 ticks = SDL_GetTicks();
    GameItem* item = NULL;
    this->mutex.lock();
    for(std::vector<GameItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        item = *it;

        item->process(ticks);

        if (item->canDelete == true) {
            it = this->items.erase(it);
            it--;   // get previous iterator

            delete item;
        }
    }
    this->mutex.unlock();
}

int GameEngine::addItem(GameItem* item) {
    this->mutex.lock();

    this->items.push_back(item);
    int itemID = item->id;

    this->mutex.unlock();
    return itemID;
}

GameItem* GameEngine::getItem(int id) {
    this->mutex.lock();
    for(std::vector<GameItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if ((*it)->id == id) {
            this->mutex.unlock();
            return *it;
        }
    }

    this->mutex.unlock();
    return NULL;
}

static Uint32 eventType = SDL_RegisterEvents(1);

bool newItem(RenderableItem* item) {
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

bool requestKeybordTarget(Uint64 id) {
    if (eventType != 0) {
        SDL_Event event;
        SDL_zero(event);
        event.type = eventType;
        event.user.code = EVENT_KEYBOARD_REQUEST;
        event.user.data1 = (void *)id;
        event.user.data2 = NULL;
        bool res = SDL_PushEvent(&event);

        if (res == false)
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());

        return true;
    }
    return false;
}