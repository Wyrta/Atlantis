#include "someFunction.hpp"


void swapBoolStates(bool *myBool) { if(*myBool) *myBool=false; else *myBool=true; };

int charToInt(const char *myChar) {
	int number = 0;
	int i = 0;

	while (myChar[i] != 0)
	{
		switch (myChar[i])
		{
			case '0': number = (number*10) + 0; break;
			case '1': number = (number*10) + 1; break;
			case '2': number = (number*10) + 2; break;
			case '3': number = (number*10) + 3; break;
			case '4': number = (number*10) + 4; break;
			case '5': number = (number*10) + 5; break;
			case '6': number = (number*10) + 6; break;
			case '7': number = (number*10) + 7; break;
			case '8': number = (number*10) + 8; break;
			case '9': number = (number*10) + 9; break;

		default: return -1; break;
		}
		i++;
	}
	return number;
}

void rectPos(SDL_Rect *myRect, SDL_Point myPos) {
	myRect->x = myPos.x;
	myRect->y = myPos.y;
}

SDL_Texture *createTexture(SDL_Renderer *render, SDL_Rect *rectangle, const char *path)
{
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	surface = IMG_Load(path);

	if(surface) {
		texture = SDL_CreateTextureFromSurface(render, surface);
		SDL_FreeSurface(surface);

		if(rectangle != NULL) SDL_QueryTexture(texture, NULL, NULL, &rectangle->w, &rectangle->h);

		return texture;
	} else cout << SDL_GetError() << endl;
	return NULL;
}

bool collision(SDL_Rect rect1, SDL_Rect rect2)
{
	if (rect1.x < rect2.x + rect2.w &&
		rect1.x + rect1.w > rect2.x &&
		rect1.y < rect2.y + rect2.h &&
		rect1.h + rect1.y > rect2.y) 
	{
		return true;
	}
	return false;
}

bool ptInRect(SDL_Point point, SDL_Rect rect)
{
	if ((point.x < (rect.x + rect.w)) & (point.x > rect.x)) {
		if ((point.y < (rect.y + rect.h)) & (point.y > rect.y)) {
			return true;
		} else return false;
	} else return false;
}

TTF_Font *createFont(const char *path, int size) {
	TTF_Font *font;
	font = TTF_OpenFont(path, size);
	if (!font) {cout << "Error with font : " << SDL_GetError() << endl; return NULL;}
	else return font;
}

SDL_Texture *write(SDL_Renderer *render, SDL_Rect *rect, TTF_Font *font, const char *text, SDL_Color color) {
	SDL_Surface *surface;

	surface = TTF_RenderText_Solid(font, text, color);

	SDL_Texture *texture = NULL;
	texture = SDL_CreateTextureFromSurface(render, surface);
	if(texture == NULL) cout << "error texture null" << endl;
	SDL_FreeSurface(surface);

	SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
	return texture;
}

Uint32 GetPixel(SDL_Surface *surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;  //bit per pixel
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
	
		case 4:
			return *(Uint32 *)p;
	
		default:
			return 0;
	}

}

int intAleat(int min, int max)  {
	if(min > max) { int temp = min; min = max; max = temp; }
	if(min == max) return max;
	return (min + (rand() % max));
}

void printDialog(SDL_Renderer *render, SDL_Rect screen, string dialog_text, string voice, int speed)
{
	SDL_Rect bgRect, bgText;
	SDL_Texture *backgroundTexture = NULL;
	SDL_Texture *text = NULL;


	/* text background */

	backgroundTexture = createTexture(render, &bgRect, "./img/background_menu.png");
	bgRect.x = screen.x;
	bgRect.y = 3*(screen.h/4);


	/* text */

	TTF_Font *fontText = createFont("./Avara.ttf", 25);
	string ttp = " ";
	text = write(render, &bgText, fontText, ttp.c_str() );
	bgText.x = bgRect.x + 50;
	bgText.y = bgRect.y + 50;
	int letterCpt = dialog_text.length() * speed;


	/* text sound */

	string talkVoicePath = "./sound/" + voice + ".ogg";
	Mix_Chunk *talkingSound = Mix_LoadWAV(talkVoicePath.c_str());


	SDL_Event event;
		SDL_Point mouse;
		int buttons;
	bool talking = true;
	while(talking)
	{
		/* event pooling */

		SDL_PollEvent(&event);
		buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
		if((buttons & SDL_BUTTON_LMASK) != 0) 
		{
			if (letterCpt > 0)
			{
				letterCpt = 1;
				do 
				{
					SDL_Delay(50);
					SDL_PollEvent(&event);
					buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
				} while ((buttons & SDL_BUTTON_LMASK) != 0);
			}
			else 
				talking = false;
		}


		/* progressive text writting */

		if(letterCpt > 0)
		{
			ttp = dialog_text.substr(0, dialog_text.length() - letterCpt/speed);
			SDL_DestroyTexture(text);
			text = write(render, &bgText, fontText, ttp.c_str() );
			if(intAleat(0, 20) == 0) /*play sound */
				Mix_PlayChannel(3, talkingSound, 0);

			letterCpt--;
		}

		SDL_RenderCopy(render, backgroundTexture, NULL, &bgRect);
		SDL_RenderCopy(render, text, NULL, &bgText);

		SDL_RenderPresent(render);
		SDL_Delay(16);
	}

	Mix_FreeChunk(talkingSound);

	SDL_DestroyTexture(backgroundTexture);
	SDL_DestroyTexture(text);

	SDL_RenderPresent(render);
}
