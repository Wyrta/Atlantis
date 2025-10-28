#ifndef EVENT_HANDLER
#define EVENT_HANDLER

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

class EventEmitter;


typedef struct Event {
    std::string type;
    Uint64 ticks;
    EventEmitter* emitter;
} Event;

class EventHandler {
private:
protected:
    std::mutex mutex;
    std::vector<Event> event;
public:
    void receiveEvent(Event event);
    virtual void handleEvent(void);
    int test = 42;
};

class EventEmitter {
private:
    EventHandler* eventHandler;

    // mouse target
    Uint64 lastClick[5];
    Uint64 mouseDown[5];

public:
    void setEventHandler(EventHandler* eventHandler);
    void sendEvent(std::string eventType);

    // mouse target 
    virtual void onHover(SDL_FPoint position) {return;};       // input
    virtual void onClick(void) {return;};
    void onDblClick(void) {return;};
    void onContextMenu(void) {return;};
    void onMouseMenu(void) {return;};
    void onMouseDown(SDL_MouseButtonEvent event);   // input
    void onMouseUp(SDL_MouseButtonEvent event);     // input
    void onMouseIn(void) {return;};
    void onMouseOut(void) {return;};

    // keyboard target
    void onKeyUp(SDL_Scancode key); 
    void onKeyDown(SDL_Scancode key);
    void onKeyHold(SDL_Scancode key);

    int keyboard = 42;
};

#endif //EVENT_HANDLER