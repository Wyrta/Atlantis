#include "EventHandler.hpp"


void EventHandler::handleEvent(void) {
    for(std::vector<Event>::iterator it = this->event.begin(); it != this->event.end(); ++it)
    {
        SDL_Log("Event handled: %s", (*it).type.c_str());
        this->event.erase(it);
    }
}

void EventHandler::receiveEvent(Event event) {
    SDL_Log("New event received: %s", event.type.c_str());
    this->event.push_back(event);
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

void MouseTarget::onMouseDown(SDL_MouseButtonEvent event) {
    this->sendEvent("onMouseDown");
}

void MouseTarget::onMouseUp(SDL_MouseButtonEvent event) {
    if (event.clicks == 1)
        this->sendEvent("onClick");
    else if (event.clicks > 1)
        this->sendEvent("onDlbClick");
    else
        this->sendEvent("onMouseDown");
}

void KeyboardTarget::onKeyUp(SDL_Scancode key) {
    SDL_Log("KeyboardTarget::onKeyUp %s", SDL_GetScancodeName(key));
    this->sendEvent("onKeyUp");
}

void KeyboardTarget::onKeyDown(SDL_Scancode key) {
    SDL_Log("KeyboardTarget::onKeyDown %s", SDL_GetScancodeName(key));
    this->sendEvent("onKeyDown");
}

void KeyboardTarget::onKeyHold(SDL_Scancode key) {
    SDL_Log("KeyboardTarget::onKeyHold %s", SDL_GetScancodeName(key));
    this->sendEvent("onKeyHold");
}