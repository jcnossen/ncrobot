#include "StdIncl.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include "OpenGL.h"
#include "GUI.h"

template <typename T>
T nextPowerOfTwo(T n)
{
	std::size_t k=1;
	--n;
	do {
		n |= n >> k;
		k <<= 1;
	} while (n & (n + 1));
	return n + 1;
}

// Very slow basic font rendering from gamedev.net
void SDL_GL_RenderText(std::string Text, TTF_Font *Font, SDL_Color Colour, SDL_Rect *Position)
{
	SDL_Surface *Initial;
	SDL_Surface *Intermediary;
	int Width, Height;
	GLuint Texture;

	//Cast the string as a non-const char*
	char *Text_NonConst = const_cast<char*>(Text.c_str());

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	//Render text to SDL surface
	Initial = TTF_RenderText_Blended(Font, Text_NonConst, Colour);

	//Get the width and height of the SDL surface containing the text
	Width = nextPowerOfTwo(Initial->w);
	Height = nextPowerOfTwo(Initial->h);

	//Create a surface with useable format for OpenGL
	Intermediary = SDL_CreateRGBSurface(0, Width, Height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	//Blit the text surface onto the OpenGL compliant one
	SDL_BlitSurface(Initial, 0, Intermediary, 0);

	//Setup the texture
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	//Copy SDL surface to the OpenGL texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, Intermediary->pixels );

	//Setup texture filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Enable textures
	glEnable(GL_TEXTURE_2D);

	//Set the colour to white (for correct blending)
	glColor3f(1.0f, 1.0f, 1.0f);

	//Draw texture using a quad
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f); glVertex2f(Position->x, Position->y + Height);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(Position->x + Width, Position->y + Height);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(Position->x + Width, Position->y);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(Position->x, Position->y);

	glEnd();

	//Disable blending
	glDisable(GL_BLEND);

	//Clean up
	SDL_FreeSurface(Initial);
	SDL_FreeSurface(Intermediary);
	glDeleteTextures(1, &Texture);
}

Font::Font() {
	ttf = 0;
}

Font::~Font() {
	if (ttf) {
		TTF_CloseFont(ttf);
		ttf=0;
	}
}

Font* Font::load(std::string fontName, int size) {
	Font *f = new Font();

	f->ttf = TTF_OpenFont(fontName.c_str(), size);
	if (!f->ttf) {
		delete f;
		throw ContentException(SPrintf("Failed to open font '%s' (size %d)", fontName.c_str(), size));
	}
	return f;
}

void Font::render(std::string text, Vector2 pos, Color c) {
	SDL_Color color;

	color.r=c.c[0];
	color.g=c.c[1];
	color.b=c.c[2];

	SDL_Rect rc;
	rc.x = pos.x;
	rc.y = pos.y;
	rc.w=rc.h = 0; // ignored
	
	SDL_GL_RenderText(text, ttf, color, &rc);
}

void GUI_Init() {
	TTF_Init();
}

void GUI_Shutdown() {
	TTF_Quit();
}

