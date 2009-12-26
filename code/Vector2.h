
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
#pragma once

#include <cmath>

struct Vector2 {
	float x,y;

	Vector2() {
		x=y=0.0f;
	}

	Vector2(float X, float Y) {
		x=X; y=Y;
	}

	explicit Vector2(float angle) {
		x=cosf(angle);
		y=sinf(angle);
	}

	Vector2 operator-() const {
		return Vector2(-x,-y);
	}

	Vector2 operator+(const Vector2& v) const {
		return Vector2(v.x+x,v.y+y);
	}

	Vector2 operator*(const Vector2& v) const {
		return Vector2(v.x*x,v.y*y);
	}

	Vector2 operator/(const Vector2& v) const {
		return Vector2(x/v.x,y/v.y);
	}

	friend Vector2 operator*(float a, const Vector2& v) {
		return Vector2(a*v.x,a*v.y);
	}

	friend Vector2 operator/(float a, const Vector2& v) {
		return Vector2(a/v.x,a/v.y);
	}

	friend Vector2 operator*(const Vector2& v, float a) {
		return Vector2(a*v.x,a*v.y);
	}

	friend Vector2 operator/(const Vector2& v, float a) {
		return Vector2(v.x/a,v.y/a);
	}

	Vector2 operator-(const Vector2& v) const {
		return Vector2(x-v.x,y-v.y);
	}

	Vector2& operator+=(const Vector2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2& operator-=(const Vector2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector2& operator*=(const Vector2& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector2& operator*=(float v) {
		x *= v;
		y *= v;
		return *this;
	}
	Vector2& operator/=(const Vector2& v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	float length2() const {
		return x*x+y*y;
	}
	float length() const {
		return sqrtf(length2());
	}

	float distance(const Vector2& v) {
		return (*this - v).length();
	}

	float normalize() {
		float len = length();
		x/=len;
		y/=len;
		return len;
	}

	Vector2 normalized() {
		Vector2 r=*this;
		r.normalize();
		return r;
	}

	// returns original length
	float setLength(float l) {
		float len = length();
		if (len!=0.0f) {
			float f = l/len;
			x*=f;
			y*=f;
		}
		return len;
	}

	Vector2 ortho() const {
		return Vector2(-y, x);
	}

	static float dot(const Vector2 & a, const Vector2& b) {
		return a.x*b.x+a.y*b.y;
	}
};


struct Box2 {
	Box2(Vector2 a, Vector2 b) : a(a), b(b) {}
	Box2() {}

	bool contains(Vector2 p) {
		return (p.x>=a.x && p.x <= b.x) &&
				(p.y>=a.y && p.y <= b.y);
	}

	Vector2 size() {
		return b-a;
	}

	void move(Vector2 v) {
		a+=v;
		b+=v;
	}

	Box2 moved(Vector2 v) {
		return Box2(a+v,b+v);		
	}

	void extend(Vector2 v) {
		if (v.x < a.x) a.x = v.x;
		if (v.x > b.x) b.x = v.x;

		if (v.y < a.y) a.y = v.y;
		if (v.y > b.y) b.y = v.y;
	}

	bool intersects(const Box2& o) {
		return a.x <= o.b.x && o.a.x <= b.x &&
			a.y <= o.b.y && o.a.y <= b.y;
	}

	// members
	Vector2 a,b;
};
