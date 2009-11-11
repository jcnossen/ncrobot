/* 
 * File:   Vector.h
 * Author: jcnossen
 *
 * Created on October 29, 2009, 10:16 AM
 */

#pragma once
#include <cassert>

namespace ncrobot {

/*
 * n-dimensional vector
 */
class Vector {
public:
	typedef float elem_t;

	Vector(int n) { d=new elem_t[n]; for(int x=0;x<n;x++)d[x]=0.0f; }
	Vector(const Vector& orig);
	~Vector() { if (d) delete[] d; }

	elem_t& operator[](int i) {
		assert(i>=0 && i<n);
		return d[i];
	}
	const elem_t& operator[](int i) const {
		assert(i>=0 && i<n);
		return d[i];
	}

	static Vector ones(int n);

	Vector operator*(const Vector& rhs) const;
	Vector operator+(const Vector& rhs) const;
	Vector operator-(const Vector& rhs) const;

	Vector& operator*=(const Vector& rhs);
	Vector& operator+=(const Vector& rhs);
	Vector& operator-=(const Vector& rhs);

	Vector operator*(elem_t a) const;
	Vector& operator*=(elem_t a);

	elem_t dotp(const Vector& rhs) const;

	int n;
	elem_t *d; // data
};

}
