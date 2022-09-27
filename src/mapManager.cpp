#include "mapManager.hpp"

extern SDL_Renderer *render;

MapManager::MapManager(string mapName) {

    string strToCut = mapName;
    int cutPos = strToCut.find('_');
    mapName = strToCut.substr(0, cutPos);

    string strDoorNum = strToCut.substr(cutPos+1, strToCut.size());
    actualDoor = stoi(strDoorNum);


    //Map texture
    textureMap = createTexture(render, &rectMap, ("./img/map/" + mapName + ".png").c_str());
    rectMap.x = 0;
    rectMap.y = 0;

    //Hitbox surface 
    hitbox = IMG_Load(("./img/map/hitbox/" + mapName + ".png").c_str());
    
    //Hitbox texture
    txtHitboxMap = createTexture(render, &rectMap, ("./img/map/hitbox/" + mapName + ".png").c_str());
    recthitboxMap.x = 0;
    recthitboxMap.y = 0;



    for (int i = 0; i < MAX_SIGN; i++) 
        sign[i] = {0, 0, 0, 0};
    
    //setup sign pos and name
    if(mapName == "plage")   {
        signNumber = 2;
        sign[0] = {900, 1850, 0, 0};
        signMapName[0] = "plage_00";
        sign[1] = {3008, 2368, 64, 64};
        signMapName[1] = "village_00";
        
    }
    if(mapName == "village") {
        signNumber = 3;
        sign[0] = {64, 1536, 64, 64}; signMapName[0] = "plage_01";
        sign[1] = {2752, 64, 64, 64}; signMapName[1] = "village_01";
        sign[2] = {0, 0, 64, 64};     signMapName[2] = "village_02"; //dont know now
    }
    //...

}

MapManager::~MapManager() {
    SDL_FreeSurface(hitbox);

    SDL_DestroyTexture(txtHitboxMap);
    SDL_DestroyTexture(textureMap);

}

SDL_Point MapManager::initPos() {
    SDL_Point mPoint;
        mPoint.x = sign[actualDoor].x;
        mPoint.y = sign[actualDoor].y;

    return mPoint;
}

bool MapManager::print(bool showHitbox) {
    recthitboxMap = rectMap;

    //Render of the map
    SDL_RenderCopy(render, textureMap, NULL, &rectMap);

    //Render of the hitbox
    if(showHitbox)
        SDL_RenderCopy(render, txtHitboxMap, NULL, &recthitboxMap);

    return true;
}

bool MapManager::isOnSign(SDL_Rect playerHitbox) {
    SDL_Rect temp;
    for (int i = 0; i < signNumber; i++) {
        //update the hitbox pos
        temp = {this->sign[i].x + this->rectMap.x, this->sign[i].y + this->rectMap.y, this->sign[i].w, this->sign[i].h};
        if(collision(playerHitbox, temp) ) {
            nextMapName = signMapName[i];
            return true;
        }      
    }
    return false;
}