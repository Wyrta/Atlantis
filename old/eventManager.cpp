#include "eventManager.hpp"

EventManager::EventManager()
{
	delay = 20;

}

EventManager::~EventManager()
{

}

void EventManager::waitForKey(int key, int state)
{
	do 
	{
		SDL_Delay(delay);
		SDL_PollEvent(&event);
	} while (keyboardState[key] != state);

	return;
}

void EventManager::waitForMouse(Uint32 button, Uint32 state)
{
	do 
	{
		SDL_Delay(delay);
		SDL_PollEvent(&event);
		mouseState = SDL_GetMouseState(&btn_x, &btn_y);
	} while ((mouseState & button) != state);

	return;
}

bool EventManager::getKey(int key)
{
	return (keyboardState[key] == true) ? (true) : (false); 
}

