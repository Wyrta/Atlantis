#include "console.hpp"

extern SDL_Renderer *render;

Console::Console(SDL_Rect *screen)
{
	size.w = screen->w / 5;
	size.h = screen->h / 5;
	size.x = 10;
	size.y =  (4*(screen->h / 5)) - 10;

	gameScreen = screen;

	for(int i = 0; i < BUFFER_LENGTH; i++)
	{
		this->buffer[i].clear();
	}

	for(int i = 0; i < MAX_LOG_LENGTH; i++)
	{
		this->lign[i] = NULL;
	}

	this->last_msg_time = SDL_GetTicks();

	this->buffer[0].assign("Welcome to Atlantis !");
}

Console::~Console()
{
	for(int i = 0; i < MAX_LOG_LENGTH; i++)
	{
		SDL_DestroyTexture(this->lign[i]);
	}
}

void Console::log(string txt_to_print)
{
	this->last_msg_time = SDL_GetTicks();

	for(int i = MAX_LOG_LENGTH-1; i >= 1; i--)
	{
		this->buffer[i] = this->buffer[i-1];
	}
	
	this->buffer[0] = txt_to_print;
}

void Console::log(const char *format, ...)
{
	va_list arglist;
	char buffer[1024];
	va_start( arglist, format );
	vsnprintf(buffer, 1024, format, arglist);
	va_end( arglist );

	string txt_to_print;
	txt_to_print.append(buffer);
	
	this->log(txt_to_print);
}


/* WARNING : need to be optimized */
void Console::print(void)
{
	if (this->last_msg_time + 5000 <= SDL_GetTicks())
		return;

	int lignToDisplay[MAX_LOG_LENGTH];
	int cpt = 0;

	size.w = this->gameScreen->w / 5;
	size.h = this->gameScreen->h / 5;
	size.x = 10;
	size.y = this->gameScreen->h - (this->gameScreen->h / 5) - 10;

	for(int i = 0; i < MAX_LOG_LENGTH; i++)
	{
		lignToDisplay[i] = -1;
		SDL_DestroyTexture(this->lign[i]);
	}

	for(int i = 0; i < BUFFER_LENGTH; i++)
	{
		if (this->buffer[i].length() > 0)
		{
			lignToDisplay[MAX_LOG_LENGTH - 1] = i;

			for(int i = 1; i < MAX_LOG_LENGTH; i++)
				lignToDisplay[i - 1] = lignToDisplay[i];

			lignToDisplay[MAX_LOG_LENGTH - 1] = -1;
		}
	}

	for(int i = 0; i < MAX_LOG_LENGTH; i++)
	{
		if (lignToDisplay[i] != -1)
		{
			this->lign[cpt]	   = write(render, &this->lignRect[cpt], DebugMenu::debugFont, this->buffer[lignToDisplay[i]].c_str(), {255, 255, 255});
			this->lignRect[cpt].x = this->size.x + 3;
			this->lignRect[cpt].y = (this->size.y + this->size.h) - ((cpt * this->lignRect[cpt].h) + this->lignRect[cpt].h);
			cpt++;
		}
	}

	SDL_SetRenderDrawColor(render, 30, 30, 30, 200);
	SDL_RenderFillRect(render, &this->size);

	for(int i = 0; i < MAX_LOG_LENGTH; i++)
	{
		if (this->lign[i] != NULL)
			SDL_RenderCopy(render, this->lign[i], NULL, &this->lignRect[i]);
	}

}
