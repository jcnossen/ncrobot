/*
ncrobot
Natural Computing Robot
This app is based on the TestBed example application from Box2D.

Copyright 2009 Jelmer Cnossen, particle swarm optimization code by Johan de Ruiter
License (ZLIB):

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/
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
