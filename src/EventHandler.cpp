#include "EventHandler.hpp"

const char *eventTypeName[] = {
    "onHover",
    "onMouseDown",
    "onMouseUp",
    "onClick",
    "onDlbClick",
    "onContextMenu",
    "onKeyDown",
    "onKeyUp",
    "onKeyHold",
    "onTextInput",

    "menuItemUpdate",

    "onNothing"   // last item
};

const char *getEventTypeName(EventType eventType) {
    if (eventType < 0)
        return eventTypeName[EventType::onNothing];

    if (eventType > EventType::onNothing)
        return eventTypeName[EventType::onNothing];

    return eventTypeName[eventType];
}

void EventHandler::handleEvent(void) {
    this->mutex.lock();
    for(std::vector<Event>::iterator it = this->events.begin(); it != this->events.end(); ++it)
    {
        SDL_Log("Event handled: %s", getEventTypeName((*it).type));
    }
    this->clearEvent();
    this->mutex.unlock();
}

void EventHandler::receiveEvent(Event newEvent) {
    this->mutex.lock();
    // SDL_Log("New event received: %s 0x%p at %ldms", getEventTypeName(newEvent.type), newEvent.emitter, newEvent.ticks);
    this->events.push_back(newEvent);
    this->mutex.unlock();
}


void EventEmitter::setEventHandler(EventHandler* eventHandler) {
    this->eventHandler = eventHandler;
}

void EventEmitter::sendEvent(Event event) {
    // SDL_Log("Event: %s", getEventTypeName(event.type));
    if (this->eventHandler == NULL)
        return;

    event.emitter = this;
    event.ticks = SDL_GetTicks();

    this->eventHandler->receiveEvent(event);
}

void EventEmitter::onMouseDown(SDL_MouseButtonEvent mouseEvent) {
    Event event;
    event.type = EventType::onMouseDown;
    
    if (mouseEvent.button == SDL_BUTTON_RIGHT)
        event.type = EventType::onContextMenu;

    this->sendEvent(event);
}

void EventEmitter::onMouseUp(SDL_MouseButtonEvent mouseEvent) {
    EventType type;
    Event event;

    if (mouseEvent.clicks == 1)
        type = EventType::onClick;
    else if (mouseEvent.clicks > 1)
        type = EventType::onDlbClick;
    else 
        type = EventType::onMouseUp;

    if (mouseEvent.button == SDL_BUTTON_RIGHT)
        return;

    event.type = type;
    this->sendEvent(event);

}

void EventEmitter::onKeyUp(SDL_KeyboardEvent keyEvent) {
    Event event;
    event.type = EventType::onKeyUp;
    event.key = keyEvent.key;
    event.scancode = keyEvent.scancode;
    this->sendEvent(event);
}

void EventEmitter::onKeyDown(SDL_KeyboardEvent keyEvent) {
    Event event;
    event.type = EventType::onKeyDown;
    event.key = keyEvent.key;
    event.scancode = keyEvent.scancode;
    this->sendEvent(event);
}

void EventEmitter::onKeyHold(SDL_KeyboardEvent keyEvent) {
    Event event;
    event.type = EventType::onKeyHold;
    event.key = keyEvent.key;
    event.scancode = keyEvent.scancode;
    this->sendEvent(event);
}

void EventEmitter::onTextInput(SDL_TextInputEvent textEvent) {
    Event event;
    event.type = EventType::onTextInput;
    event.text = std::string(textEvent.text);
    this->sendEvent(event);
}
