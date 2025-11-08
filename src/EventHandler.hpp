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

typedef enum EventType {
    onHover = 0,
    onMouseDown,
    onMouseUp,
    onClick,
    onDlbClick,
    onContextMenu,
    onKeyDown,
    onKeyUp,
    onKeyHold,
    onTextInput,

    menuItemUpdate,

    onNothing   // last item
} EventType;

const char *getEventTypeName(EventType eventType);

typedef struct Event {
    EventType type;
    Uint64 ticks;
    EventEmitter* emitter;

    SDL_Keycode key;
    SDL_Scancode scancode;
    std::string text;

    std::string value;

    SDL_FPoint mousePos;
    SDL_MouseButtonFlags mouseFlags;
} Event;

class EventHandler {
private:
protected:
    std::mutex mutex;
    std::vector<Event> events;
public:
    void receiveEvent(Event event);
    virtual void handleEvent(void);
    void clearEvent(void) {this->events.clear();};

};

class EventEmitter {
private:
    EventHandler* eventHandler;

public:
    void setEventHandler(EventHandler* eventHandler);
    void sendEvent(Event event);

    // mouse target 
    virtual void onHover(SDL_FPoint position) {return;};       // input
    virtual void onClick(void) {return;};
    void onDblClick(void) {return;};
    void onContextMenu(void) {return;};
    void onMouseDown(SDL_MouseButtonEvent event);   // input
    void onMouseUp(SDL_MouseButtonEvent event);     // input
    void onMouseIn(void) {return;};
    void onMouseOut(void) {return;};

    // keyboard target
    void onKeyUp(SDL_KeyboardEvent event);
    void onKeyDown(SDL_KeyboardEvent event);
    void onKeyHold(SDL_KeyboardEvent event);

    void onTextInput(SDL_TextInputEvent event);


    int keyboard = 42;
};

#endif //EVENT_HANDLER