#include "EventHandler.hpp"


void EventHandler::handleEvent(void) {
    this->mutex.lock();
    for(std::vector<Event>::iterator it = this->event.begin(); it != this->event.end(); ++it)
    {
        SDL_Log("Event handled: %s", (*it).type.c_str());
        it = this->event.erase(it);
        it--;   // get previous iterator
    }
    this->mutex.unlock();
}

void EventHandler::receiveEvent(Event newEvent) {
    this->mutex.lock();
    SDL_Log("New event received: %s 0x%p at %ldms", newEvent.type.c_str(), newEvent.emitter, newEvent.ticks);
    this->event.push_back(newEvent);
    this->mutex.unlock();
}


void EventEmitter::setEventHandler(EventHandler* eventHandler) {
    this->eventHandler = eventHandler;
}

void EventEmitter::sendEvent(std::string eventType) {
    // SDL_Log("Event: %s", eventType.c_str());
    if (this->eventHandler == NULL)
        return;

    Event event;
    event.type = eventType;
    event.emitter = this;
    event.ticks = SDL_GetTicks();
    this->eventHandler->receiveEvent(event);
}

void EventEmitter::onMouseDown(SDL_MouseButtonEvent event) {
    this->sendEvent("onMouseDown");
}

void EventEmitter::onMouseUp(SDL_MouseButtonEvent event) {
    if (event.clicks == 1)
        this->sendEvent("onClick");
    else if (event.clicks > 1)
        this->sendEvent("onDlbClick");
    else
        this->sendEvent("onMouseDown");
}

void EventEmitter::onKeyUp(SDL_Scancode key) {
    SDL_Log("EventEmitter::onKeyUp %s", SDL_GetScancodeName(key));
    this->sendEvent("onKeyUp");
}

void EventEmitter::onKeyDown(SDL_Scancode key) {
    SDL_Log("EventEmitter::onKeyDown %s", SDL_GetScancodeName(key));
    this->sendEvent("onKeyDown");
}

void EventEmitter::onKeyHold(SDL_Scancode key) {
    SDL_Log("EventEmitter::onKeyHold %s", SDL_GetScancodeName(key));
    this->sendEvent("onKeyHold");
}