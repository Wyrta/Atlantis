#include "debugMenu.hpp"

DebugMenu::DebugMenu(SDL_Renderer *render) {
    debugFont = createFont("./Avara.ttf", 16);

    fpsDisplay  = write(render, &rectFpsDisplay, debugFont, "FPS:");
        rectFpsDisplay.x = 0; rectFpsDisplay.y = 0;
    fpsDisplay2 = write(render, &rectFpsDisplay2, debugFont, "358105");
        rectFpsDisplay2.x = rectFpsDisplay.w + 3;
        rectFpsDisplay2.y = 0;

    texturePosPLayer = write(render, &rectTexturePosPLayer, debugFont, "Position (X, Y) :");
        rectTexturePosPLayer.x = 0;
        rectTexturePosPLayer.y = rectFpsDisplay.h + 5;
    texturePosPLayerX = write(render, &rectTexturePosPLayerX, debugFont, "_");
        rectTexturePosPLayerX.x = 0;
        rectTexturePosPLayerX.y = rectTexturePosPLayer.x + rectTexturePosPLayer.w;
    texturePosPLayerY = write(render, &rectTexturePosPLayerY, debugFont, "_");
        rectTexturePosPLayerY.x = 0;
        rectTexturePosPLayerY.y = rectTexturePosPLayerX.x + rectTexturePosPLayerX.w;
}
void DebugMenu::print(SDL_Renderer *render, int fps, SDL_Point playerCoo) {
    //FPS
    SDL_DestroyTexture(fpsDisplay2);
    fpsDisplay2 = write(render, &rectFpsDisplay2, debugFont, to_string(fps).c_str());

    rectFpsDisplay2.x = rectFpsDisplay.w + 3;
    rectFpsDisplay2.y = 0;

    SDL_RenderCopy(render, fpsDisplay, NULL, &rectFpsDisplay);
    SDL_RenderCopy(render, fpsDisplay2, NULL, &rectFpsDisplay2);


    //PLAYER POSITION
    SDL_DestroyTexture(texturePosPLayerX);
    SDL_DestroyTexture(texturePosPLayerY);

    texturePosPLayerX = write(render, &rectTexturePosPLayerX, debugFont, to_string(playerCoo.x).c_str());
        rectTexturePosPLayerX.x = rectTexturePosPLayer.y;
        rectTexturePosPLayerX.y = rectTexturePosPLayer.y + rectTexturePosPLayer.h;
    texturePosPLayerY = write(render, &rectTexturePosPLayerY, debugFont, to_string(playerCoo.y).c_str());
        rectTexturePosPLayerY.x = rectTexturePosPLayer.y;
        rectTexturePosPLayerY.y = rectTexturePosPLayerX.y + rectTexturePosPLayerX.h;

    SDL_RenderCopy(render, texturePosPLayer, NULL, &rectTexturePosPLayer);
    SDL_RenderCopy(render, texturePosPLayerX, NULL, &rectTexturePosPLayerX);
    SDL_RenderCopy(render, texturePosPLayerY, NULL, &rectTexturePosPLayerY);
}

