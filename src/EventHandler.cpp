#include "EventHandler.hpp"

const char *eventTypeName[] = {
    "onHover",
    "onMouseDown",
    "onMouseUp",
    "onClick",
    "onDlbClick",
    "onKeyDown",
    "onKeyUp",
    "onKeyHold",
    "onTextInput",

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
    for(std::vector<Event>::iterator it = this->event.begin(); it != this->event.end(); ++it)
    {
        SDL_Log("Event handled: %s", getEventTypeName((*it).type));
        it = this->event.erase(it);
        it--;   // get previous iterator
    }
    this->mutex.unlock();
}

void EventHandler::receiveEvent(Event newEvent) {
    this->mutex.lock();
    // SDL_Log("New event received: %s 0x%p at %ldms", getEventTypeName(newEvent.type), newEvent.emitter, newEvent.ticks);
    this->event.push_back(newEvent);
    this->mutex.unlock();
}


void EventEmitter::setEventHandler(EventHandler* eventHandler) {
    this->eventHandler = eventHandler;
}

void EventEmitter::sendEvent(Event event) {
    // SDL_Log("Event: %s", eventTypeName(event.type));
    if (this->eventHandler == NULL)
        return;

    event.emitter = this;
    event.ticks = SDL_GetTicks();

    this->eventHandler->receiveEvent(event);
}

void EventEmitter::onMouseDown(SDL_MouseButtonEvent mouseEvent) {
    Event event;
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
        type = EventType::onMouseDown;

    this->sendEvent(event);

}

void EventEmitter::onKeyUp(SDL_Keycode key) {
    Event event;
    event.type = EventType::onKeyUp;
    event.key = key;
    this->sendEvent(event);
}

void EventEmitter::onKeyDown(SDL_Keycode key) {
    Event event;
    event.type = EventType::onKeyDown;
    event.key = key;
    this->sendEvent(event);
}

void EventEmitter::onKeyHold(SDL_Keycode key) {
    Event event;
    event.type = EventType::onKeyHold;
    event.key = key;
    this->sendEvent(event);
}

void EventEmitter::onTextInput(std::string text) {
    Event event;
    event.type = EventType::onTextInput;
    event.text = text;
    this->sendEvent(event);
}
