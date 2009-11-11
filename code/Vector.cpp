/* 
 * File:   Vector.cpp
 * Author: jcnossen
 * 
 * Created on October 29, 2009, 10:16 AM
 */

#include "StdIncl.h"
#include "Vector.h"

namespace ncrobot {

	Vector::Vector(const Vector& orig) {
		d=new elem_t[orig.n];
		n=orig.n;
		for(int x=0;x<n;x++)
			d[x]=orig.d[x];
	}

	#define ELEMOP(_op) \
		Vector Vector::operator _op(const Vector& rhs) const { \
			assert(rhs.n==n);               \
			Vector r(n);                    \
			for(int x=0;x<n;x++)            \
				r[x] = rhs.d[x] _op d[x];   \
			return r; \
		}

	ELEMOP(*)
	ELEMOP(+)
	ELEMOP(-)

	#undef ELEMOP

	#define ELEM_ASSIGNOP(_op) \
		Vector& Vector::operator _op(const Vector& rhs) { \
			assert(rhs.n==n);                           \
			for(int x=0;x<n;x++) d[x] _op rhs.d[x];     \
			return *this;                               \
		}

	ELEM_ASSIGNOP(*=)
	ELEM_ASSIGNOP(+=)
	ELEM_ASSIGNOP(-=)

	#undef ELEM_ASSIGNOP

	Vector& Vector::operator *=(Vector::elem_t a) {
		for(int x=0;x<n;x++)
			d[x]*=a;
		return *this;
	}

	Vector Vector::operator *(Vector::elem_t a) const {
		Vector r(n);
		for(int x=0;x<n;x++)
			r.d[x]=d[x]*a;
		return r;
	}

	Vector::elem_t Vector::dotp(const Vector& rhs) const {
		elem_t accum=0.0f;
		assert(rhs.n == n);
		for(int x=0;x<n;x++)
			accum += d[x]*rhs.d[x];
		return accum;
	}

	Vector Vector::ones(int n) {
		Vector r(n);
		for(int x=0;x<n;x++)
			r[x]=1.0f;
		return r;
	}

}