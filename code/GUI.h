#pragma once

#include <SDL_ttf.h>

class Font {
public:
	Font();
	~Font();

	static Font* load(std::string fontName, int size);

	void render(std::string text, Vector2 pos, Color c);

private:
	TTF_Font* ttf;
};


void GUI_Init();
void GUI_Shutdown();


// Slow direct font rendering
void SDL_GL_RenderText(std::string Text, TTF_Font *Font, SDL_Color Colour, SDL_Rect *Position);
