#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP


#include "../include/SDL/SDL.h"
#include "someFunction.hpp"

using namespace std;


class EventManager {
	public:
	EventManager();
	~EventManager();

	/**
	 * @param button SDL_BUTTON_LMASK | SDL_BUTTON_RMASK
	 * @param state  up/down 1/0
	 */
	void waitForMouse(Uint32 button, Uint32 state);

	/**
	 * @param key 	enum SDLK_
	 * @param state up/down 1/0
	 */
	void waitForKey(int key, int state);

	/**
	 * @param key enum SDLK_
	 */
	bool getKey(int key);

	private:
	int delay;
	SDL_Event event;
	const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

	Uint32 mouseState;
	int btn_x, btn_y;
};

#endif /* EVENTMANAGER_HPP */
