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

	mouse.wheel = 0;

	/* store last value of the keyboard */
	for (int i = 0; i < SDL_Scancode::SDL_NUM_SCANCODES; i++)
	{
		this->last_keyboardState[i] = this->keyboardState[i];
	}
	
	/* event proc */
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
					log = false;
				} break;
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				{
					Uint32 mask = SDL_GetMouseState(&mouse.x, &mouse.y);
					mouse.left  = (mask & SDL_BUTTON_LMASK);
					mouse.right = (mask & SDL_BUTTON_RMASK);
					log = false;
				} break;
				case SDL_MOUSEWHEEL:
				{
					if(event.wheel.y > 0) // scroll up
					{
						// console->log("Wheel UP");	// Put code for handling "scroll up" here!
						mouse.wheel++;
					}
					else if(event.wheel.y < 0) // scroll down
					{
						// console->log("Wheel DOWN");	// Put code for handling "scroll down" here!
						mouse.wheel--;
					}

					if(event.wheel.x > 0) // scroll right
					{
						console->log("Wheel RIGHT");	// ...
					}
					else if(event.wheel.x < 0) // scroll left
					{
						console->log("Wheel LEFT");	// ...
					}
					log = false;
				}break;
				case SDL_KEYDOWN: {
					if (this->getKeySC(SDL_SCANCODE_ESCAPE))	/* ESCAPE */
						*this->appState = EXIT;
					log = false;
				} break;
				case SDL_KEYUP: {

					log = false;
				} break;

				default: log = false; break;
			}

				/* dont need to log */


			if (log)
			{
				if (eventPolling > 0)
					console->log(EVENT, "nb poll: %d", eventPolling);
				else
					console->log(EVENT, "");
			}
		}
		
		eventPolling++;
	} while ((eventPolling < MAX_EVENT_POLL) && (hasEvent == 1));
}

bool EventManager::getKeySC(SDL_Scancode key)
{
	return (this->keyboardState[key]); 
}

bool EventManager::getKeyUp(SDL_Scancode key)
{
	bool retval;

	retval = (this->last_keyboardState[key] == false) && (this->keyboardState[key] == true);

	return (retval);
}


bool EventManager::getKeyDown(SDL_Scancode key)
{
	bool retval;

	retval = (this->last_keyboardState[key] == true) && (this->keyboardState[key] == false);

	return (retval);
}

