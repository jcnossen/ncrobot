
// Include OpenGL + utility functions
#pragma once

#include "Vector2.h"
#include "Color.h"

#include <SDL_opengl.h>

inline void glVertex(Vector2 v) {
	glVertex2f(v.x,v.y);
}

inline void glColor(Color color) {
	glColor4ubv((GLubyte*)color.c);
}

inline void glColorRGB(int rgb) {
	glColor3ub( (rgb & 0xff0000) >> 16, (rgb & 0xff00) >> 8, (rgb & 0xff));
}
