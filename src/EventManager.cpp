#include "EventManager.hpp"

#include "Console.hpp"

extern Console *console; 

EventManager::EventManager(State *state)
{
	this->appState = state;
}

EventManager::~EventManager()
{
}


void EventManager::pollEvent(void)
{
	int eventPolling = 0;
	int hasEvent	 = 0;
	
	bool log = true; 

	do
	{
		hasEvent = SDL_PollEvent(&this->event);
		
		if (hasEvent) {

			switch (this->event.type)
			{
				case SDL_QUIT: *this->appState = EXIT; break;

				case SDL_MOUSEBUTTONDOWN:	break;
				case SDL_MOUSEBUTTONUP:		break;
				case SDL_MOUSEWHEEL:		break;
				case SDL_KEYDOWN: {
					/* ESCAPE */
					if (this->getKey(SDL_SCANCODE_ESCAPE))
						*this->appState = EXIT;
				} break;
				case SDL_KEYUP:				break;

				default: log = false; break;
			}

			if (log)
				console->log(EVENT, "%d", eventPolling);
		}
		
		eventPolling++;
	} while ((eventPolling < MAX_EVENT_POLL) && (hasEvent == 1));
}

bool EventManager::getKey(SDL_Scancode key)
{
	return (keyboardState[key]); 
}