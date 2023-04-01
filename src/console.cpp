#include "Console.hpp"
#include "EventManager.hpp"

#include <iostream>
#include <fstream> 
#include <chrono>

extern SDL_Renderer *render;
extern EventManager *event;
extern int 			i_tick;

Console::Console(SDL_Rect *screen)
{
	this->last_msg_time = SDL_GetTicks();
    this->first_log     = 0;
    this->last_log      = 0;

	this->buffer[0].assign("Console init");

	const int64_t time_ms = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch() ).count();
	
	char filename[64];
	snprintf(filename, 64, "logs/atlantis_%I64d.log", time_ms);

	logFile = new ofstream(filename);
}

Console::~Console()
{
	logFile->close();
	delete (logFile);
}

void Console::log(const char *format, ...)
{
	/* args parsing */
	va_list arglist;
	char buff[BUF_LENGTH];
	va_start(arglist, format);
	vsnprintf(buff, BUF_LENGTH, format, arglist);
	va_end(arglist);

	this->log(MSG, "%s", buff);
}

void Console::log(log_t type, const char *format, ...)
{
	/* args parsing */
	va_list arglist;
	char buff[BUF_LENGTH], message[BUF_LENGTH], type_str[16];
	va_start(arglist, format);
	vsnprintf(buff, BUF_LENGTH, format, arglist);
	va_end(arglist);

	const int64_t time_ms = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch() ).count();
	const int hour	= (int) ((time_ms / (1000*60*60)) % 24);
	const int min	= (int) ((time_ms / (1000*60))    % 60);
	const int sec	= (int) (time_ms  / 1000)		  % 60 ;
	const int ms	= time_ms % 1000;

	this->last_msg_time = SDL_GetTicks();
	SDL_memset(message, 0, BUF_LENGTH);
	SDL_memset(type_str, 0, 16);

	this->last_log++;

	if (this->last_log == LOG_LENGHT)
		this->last_log = 0;

	if (this->last_log == this->first_log)
	{
		this->first_log++;
		if (this->first_log == LOG_LENGHT)
			this->first_log = 0;
	}

	switch (type)
	{
		case MSG: 				snprintf(type_str, 16, "LOG"); break;
		case EVENT: 			snprintf(type_str, 16, "EVT"); break;
		case ERROR: 			snprintf(type_str, 16, "ERR"); break;
		case CRITICAL_ERROR: 	snprintf(type_str, 16, "CER"); break;
		case WARNING: 			snprintf(type_str, 16, "WRN"); break;
		default: snprintf(type_str, 16, "???"); break;
	}

	if (type == EVENT)
	{
		char event_type[32];
		switch (event->event.type)
		{
			case SDL_FIRSTEVENT:				sprintf(event_type, "SDL_FIRSTEVENT"); break;
			case SDL_QUIT:						sprintf(event_type, "SDL_QUIT"); break;
			case SDL_APP_TERMINATING:			sprintf(event_type, "SDL_APP_TERMINATING"); break;
			case SDL_APP_LOWMEMORY:				sprintf(event_type, "SDL_APP_LOWMEMORY"); break;
			case SDL_APP_WILLENTERBACKGROUND:	sprintf(event_type, "SDL_APP_WILLENTERBACKGROUND"); break;
			case SDL_APP_DIDENTERBACKGROUND:	sprintf(event_type, "SDL_APP_DIDENTERBACKGROUND"); break;
			case SDL_APP_WILLENTERFOREGROUND:	sprintf(event_type, "SDL_APP_WILLENTERFOREGROUND"); break;
			case SDL_APP_DIDENTERFOREGROUND:	sprintf(event_type, "SDL_APP_DIDENTERFOREGROUND"); break;
			case SDL_LOCALECHANGED:				sprintf(event_type, "SDL_LOCALECHANGED"); break;
			case SDL_DISPLAYEVENT:				sprintf(event_type, "SDL_DISPLAYEVENT"); break;
			case SDL_WINDOWEVENT:				sprintf(event_type, "SDL_WINDOWEVENT"); break;
			case SDL_SYSWMEVENT:				sprintf(event_type, "SDL_SYSWMEVENT"); break;
			case SDL_KEYDOWN:					sprintf(event_type, "SDL_KEYDOWN"); break;
			case SDL_KEYUP:						sprintf(event_type, "SDL_KEYUP"); break;
			case SDL_TEXTEDITING:				sprintf(event_type, "SDL_TEXTEDITING"); break;
			case SDL_TEXTINPUT:					sprintf(event_type, "SDL_TEXTINPUT"); break;
			case SDL_KEYMAPCHANGED:				sprintf(event_type, "SDL_KEYMAPCHANGED"); break;
			case SDL_MOUSEMOTION:				sprintf(event_type, "SDL_MOUSEMOTION"); break;
			case SDL_MOUSEBUTTONDOWN:			sprintf(event_type, "SDL_MOUSEBUTTONDOWN"); break;
			case SDL_MOUSEBUTTONUP:				sprintf(event_type, "SDL_MOUSEBUTTONUP"); break;
			case SDL_MOUSEWHEEL:				sprintf(event_type, "SDL_MOUSEWHEEL"); break;
			case SDL_JOYAXISMOTION:				sprintf(event_type, "SDL_JOYAXISMOTION"); break;
			case SDL_JOYBALLMOTION:				sprintf(event_type, "SDL_JOYBALLMOTION"); break;
			case SDL_JOYHATMOTION:				sprintf(event_type, "SDL_JOYHATMOTION"); break;
			case SDL_JOYBUTTONDOWN:				sprintf(event_type, "SDL_JOYBUTTONDOWN"); break;
			case SDL_JOYBUTTONUP:				sprintf(event_type, "SDL_JOYBUTTONUP"); break;
			case SDL_JOYDEVICEADDED:			sprintf(event_type, "SDL_JOYDEVICEADDED"); break;
			case SDL_JOYDEVICEREMOVED:			sprintf(event_type, "SDL_JOYDEVICEREMOVED"); break;
			case SDL_CONTROLLERAXISMOTION:		sprintf(event_type, "SDL_CONTROLLERAXISMOTION"); break;
			case SDL_CONTROLLERBUTTONDOWN:		sprintf(event_type, "SDL_CONTROLLERBUTTONDOWN"); break;
			case SDL_CONTROLLERBUTTONUP:		sprintf(event_type, "SDL_CONTROLLERBUTTONUP"); break;
			case SDL_CONTROLLERDEVICEADDED:		sprintf(event_type, "SDL_CONTROLLERDEVICEADDED"); break;
			case SDL_CONTROLLERDEVICEREMOVED:	sprintf(event_type, "SDL_CONTROLLERDEVICEREMOVED"); break;
			case SDL_CONTROLLERDEVICEREMAPPED:	sprintf(event_type, "SDL_CONTROLLERDEVICEREMAPPED"); break;
			case SDL_CONTROLLERTOUCHPADDOWN:	sprintf(event_type, "SDL_CONTROLLERTOUCHPADDOWN"); break;
			case SDL_CONTROLLERTOUCHPADMOTION:	sprintf(event_type, "SDL_CONTROLLERTOUCHPADMOTION"); break;
			case SDL_CONTROLLERTOUCHPADUP:		sprintf(event_type, "SDL_CONTROLLERTOUCHPADUP"); break;
			case SDL_CONTROLLERSENSORUPDATE:	sprintf(event_type, "SDL_CONTROLLERSENSORUPDATE"); break;
			case SDL_FINGERDOWN:				sprintf(event_type, "SDL_FINGERDOWN"); break;
			case SDL_FINGERUP:					sprintf(event_type, "SDL_FINGERUP"); break;
			case SDL_FINGERMOTION:				sprintf(event_type, "SDL_FINGERMOTION"); break;
			case SDL_DOLLARGESTURE:				sprintf(event_type, "SDL_DOLLARGESTURE"); break;
			case SDL_DOLLARRECORD:				sprintf(event_type, "SDL_DOLLARRECORD"); break;
			case SDL_MULTIGESTURE:				sprintf(event_type, "SDL_MULTIGESTURE"); break;
			case SDL_CLIPBOARDUPDATE:			sprintf(event_type, "SDL_CLIPBOARDUPDATE"); break;
			case SDL_DROPFILE:					sprintf(event_type, "SDL_DROPFILE"); break;
			case SDL_DROPTEXT:					sprintf(event_type, "SDL_DROPTEXT"); break;
			case SDL_DROPBEGIN:					sprintf(event_type, "SDL_DROPBEGIN"); break;
			case SDL_DROPCOMPLETE:				sprintf(event_type, "SDL_DROPCOMPLETE"); break;
			case SDL_AUDIODEVICEADDED:			sprintf(event_type, "SDL_AUDIODEVICEADDED"); break;
			case SDL_AUDIODEVICEREMOVED:		sprintf(event_type, "SDL_AUDIODEVICEREMOVED"); break;
			case SDL_SENSORUPDATE:				sprintf(event_type, "SDL_SENSORUPDATE"); break;
			case SDL_RENDER_TARGETS_RESET:		sprintf(event_type, "SDL_RENDER_TARGETS_RESET"); break;
			case SDL_RENDER_DEVICE_RESET:		sprintf(event_type, "SDL_RENDER_DEVICE_RESET"); break;
			case SDL_USEREVENT:					sprintf(event_type, "SDL_USEREVENT"); break;
			case SDL_LASTEVENT:					sprintf(event_type, "SDL_LASTEVENT"); break;

			default: sprintf(event_type, "Unkonw event"); break;
		}

		snprintf(message, BUF_LENGTH, "%02d:%02d:%02d:%03d %06d [%s] : %s %s", hour, min, sec, ms, i_tick, type_str, event_type, buff);
	}
	else
	{
		snprintf(message, BUF_LENGTH, "%02d:%02d:%02d:%03d %06d [%s] : %s", hour, min, sec, ms, i_tick, type_str, buff);
	}


	this->buffer[this->last_log].assign(message);

	cout << this->buffer[this->last_log] << endl;
	*this->logFile << this->buffer[this->last_log] << endl;
}


void Console::print_all(void) {
	int cpt = this->first_log;

	cout << "[Console] first " << this->first_log << ", last " << this->last_log << endl;

	for (cpt = 0; cpt < LOG_LENGHT; cpt++)
	{
		cout << "\t" << cpt << " :\t\"" << this->buffer[cpt] << "\"" << endl;
	}
}