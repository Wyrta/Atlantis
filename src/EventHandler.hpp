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
public:
    MouseTarget();

    void onHover(void) {return;};
    void onClick(void) {return;};
    void onDblClick(void) {return;};
    void onContextMenu(void) {return;};
    void onMouseMenu(void) {return;};
    void onMouseDown(void) {return;};
    void onMouseUp(void) {return;};
    void onMouseIn(void) {return;};
    void onMouseOut(void) {return;};
};


#endif //EVENT_HANDLER