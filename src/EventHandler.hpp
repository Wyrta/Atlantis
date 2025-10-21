#ifndef EVENT_HANDLER
#define EVENT_HANDLER

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>

class MouseTarget {
private:
    Uint64 lastClick[5];
    Uint64 mouseDown[5];
public:
    void onHover(SDL_FPoint position) {return;};       // input
    void onClick(void) {return;};
    void onDblClick(void) {return;};
    void onContextMenu(void) {return;};
    void onMouseMenu(void) {return;};
    void onMouseDown(int btnId) {return;};   // input
    void onMouseUp(int btnId) {return;};     // input
    void onMouseIn(void) {return;};
    void onMouseOut(void) {return;};
};


#endif //EVENT_HANDLER