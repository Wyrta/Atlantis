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
    onKeyDown,
    onKeyUp,
    onKeyHold,
    onTextInput,

    onNothing   // last item
} EventType;

const char *getEventTypeName(EventType eventType);

typedef struct Event {
    EventType type;
    Uint64 ticks;
    EventEmitter* emitter;

    SDL_Keycode key;
    std::string text;
} Event;

class EventHandler {
private:
protected:
    std::mutex mutex;
    std::vector<Event> event;
public:
    void receiveEvent(Event event);
    virtual void handleEvent(void);
};

class EventEmitter {
private:
    EventHandler* eventHandler;

    // mouse target
    Uint64 lastClick[5];
    Uint64 mouseDown[5];

public:
    void setEventHandler(EventHandler* eventHandler);
    void sendEvent(Event event);

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
    void onKeyUp(SDL_Keycode key);
    void onKeyDown(SDL_Keycode key);
    void onKeyHold(SDL_Keycode key);

    void onTextInput(std::string text);


    int keyboard = 42;
};

#endif //EVENT_HANDLER