#ifndef _console_hpp_
#define _console_hpp_

#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#define BUF_LENGTH      1024
#define LOG_LENGHT      16

using namespace std;

typedef enum { 
	MSG,
	EVENT,
	ERROR,
	CRITICAL_ERROR,
	WARNING,
} log_t ;

class Console {
	public:
		Console(SDL_Rect *);
		~Console();

		void log(const char *, ...);
		void log(log_t , const char *, ...);

		void print_all(void);


	private:
		string buffer[BUF_LENGTH];

		Uint32 last_msg_time;
        int first_log, last_log;
};

#endif /* _console_hpp_ */