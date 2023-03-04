#include "EventManager.hpp"

#include "Console.hpp"

extern Console		*console;
extern SDL_Window	*window;
extern SDL_Rect		screen;
extern Mouse_t		mouse;


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
				case SDL_MOUSEMOTION: {
					SDL_GetMouseState(&mouse.x, &mouse.y);
					log = false;	/* dont need to log this on */
				} break;
				case SDL_MOUSEBUTTONDOWN: {
					Uint32 mask = SDL_GetMouseState(&mouse.x, &mouse.y);
					mouse.left  = (mask & SDL_BUTTON_LMASK);
					mouse.right = (mask & SDL_BUTTON_RMASK);
				} break;
				case SDL_MOUSEBUTTONUP: {
					Uint32 mask = SDL_GetMouseState(&mouse.x, &mouse.y);
					mouse.left  = (mask & SDL_BUTTON_LMASK);
					mouse.right = (mask & SDL_BUTTON_RMASK);
				} break;
				case SDL_MOUSEWHEEL:		break;
				case SDL_KEYDOWN: {
					if (this->getKey(SDL_SCANCODE_ESCAPE))	/* ESCAPE */
						*this->appState = EXIT;
					if (this->getKey(SDL_SCANCODE_F3))	/* F3 */
						console->log("x%d y%d l%d, r%d", mouse.x, mouse.y, mouse.left, mouse.right);
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