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
