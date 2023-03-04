#include "EventManager.hpp"

#include "Console.hpp"

extern Console		*console;
extern SDL_Window	*window;
extern SDL_Rect		screen;


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
				case SDL_WINDOWEVENT: {
					if (this->event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
						SDL_GetWindowSize(window, &screen.w, &screen.h);
				} break;
				case SDL_MOUSEBUTTONDOWN:	break;
				case SDL_MOUSEBUTTONUP:		break;
				case SDL_MOUSEWHEEL:		break;
				case SDL_KEYDOWN: {
					if (this->getKey(SDL_SCANCODE_ESCAPE))	/* ESCAPE */
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