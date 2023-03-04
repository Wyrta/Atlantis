#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"
#include "debugMenu.hpp"

#define BUFFER_LENGTH   16
#define MAX_LOG_LENGTH  8


using namespace std;


class Console {
	public:
		Console(SDL_Rect *screen);
		~Console();

		void log(string txt_to_print);
		void log(const char *args, ...);

		void print(void);
		void input(void);

	private:
		string buffer[BUFFER_LENGTH];

		SDL_Rect size;
		SDL_Rect *gameScreen; 

		SDL_Texture *lign[MAX_LOG_LENGTH];
		SDL_Rect	 lignRect[MAX_LOG_LENGTH];

		Uint32 last_msg_time;
};

#endif //CONSOLE_HPP