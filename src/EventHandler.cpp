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
    SDL_Log("New event received: %s 0x%p at %ldms", getEventTypeName(newEvent.type), newEvent.emitter, newEvent.ticks);
    this->event.push_back(newEvent);
    this->mutex.unlock();
}


void EventEmitter::setEventHandler(EventHandler* eventHandler) {
    this->eventHandler = eventHandler;
}

void EventEmitter::sendEvent(EventType eventType, auto data) {
    // SDL_Log("Event: %s", eventType.c_str());
    if (this->eventHandler == NULL)
        return;

    Event event;
    event.type = eventType;
    event.emitter = this;
    event.ticks = SDL_GetTicks();

    switch (eventType)
    {
    case EventType::onKeyUp:
    case EventType::onKeyDown:
    case EventType::onKeyHold:
        event.key = (SDL_Scancode)data;
        break;
    
    default:
        break;
    }

    this->eventHandler->receiveEvent(event);
}

void EventEmitter::onMouseDown(SDL_MouseButtonEvent event) {
    this->sendEvent(EventType::onMouseDown, NULL);
}

void EventEmitter::onMouseUp(SDL_MouseButtonEvent event) {
    if (event.clicks == 1)
        this->sendEvent(EventType::onClick, NULL);
    else if (event.clicks > 1)
        this->sendEvent(EventType::onDlbClick, NULL);
    else
        this->sendEvent(EventType::onMouseDown, NULL);
}

void EventEmitter::onKeyUp(SDL_Scancode key) {
    SDL_Log("EventEmitter::onKeyUp %s", SDL_GetScancodeName(key));
    this->sendEvent(EventType::onKeyUp, key);
}

void EventEmitter::onKeyDown(SDL_Scancode key) {
    SDL_Log("EventEmitter::onKeyDown %s", SDL_GetScancodeName(key));
    this->sendEvent(EventType::onKeyDown, key);
}

void EventEmitter::onKeyHold(SDL_Scancode key) {
    SDL_Log("EventEmitter::onKeyHold %s", SDL_GetScancodeName(key));
    this->sendEvent(EventType::onKeyHold, key);
}