#ifndef SOMEFUNCTION_H_
#define SOMEFUNCTION_H_

#include <iostream>
#include <string>
#include <fstream>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#define SIZEOF_ARRAY(x)  (sizeof(x) / sizeof((x)[0]))

using namespace std;


/**
 * Swap the bool states :
 *      true become false
 *      false become true
 * @param myBool ptr of the bool to reverse
*/
void swapBoolStates(bool *);

/**
 *  Convert an char* into int
 *  Return the convert value and -1 if error
 *  @param ptr_myChar char to convert, musn't be a letter
 */
int charToInt(const char *);

/**
 *  Change the X and Y of a rect
 *  @param ptr_myRect dst rect
 *  @param myPos new X and Y
 */
void rectPos(SDL_Rect *myRect, SDL_Point myPos);

/** 
 * Create a texture with file path and modify the rectangle
 * 
 * @param ptr_render the render
 * @param ptr_rectangle rectangle of the texture
 * @param ptr_path name of the texure file
 * @return the textures ptr
 */
SDL_Texture *createTexture(SDL_Renderer *render, SDL_Rect *rect, const char *path);

/**
 * @return true if the rects are touching
 * @param rect1 
 * @param rect2
*/
bool collision(SDL_Rect rect1, SDL_Rect rect2);

/**
 * @return true if the rects are touching
 * @param point
 * @param rect
*/
bool ptInRect(SDL_Point point, SDL_Rect rect);

/**
 * @return A SDL Font 
 * @param path font file path
 * @param size size in px
*/
TTF_Font *createFont(const char *path, int size);

/**
 * @return texture of the text
 * @param render the render
 * @param rect the textures rect
 * @param font SDL font
 * @param text text to display
*/
SDL_Texture *write(SDL_Renderer *render, SDL_Rect *rect, TTF_Font *font, const char *text);

/**
 * @return Return the pixels color
 * @param surface suface of the pixel
 * @param x pos in X of the pixel
 * @param y pos in Y of the pixel
*/
Uint32 GetPixel(SDL_Surface *surface, int x, int y);


#endif //SOMEFUNCTION_H_