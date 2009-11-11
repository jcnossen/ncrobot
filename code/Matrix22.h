#pragma once

#include "Vector2.h"



class Matrix22 {
public:
	float v[4];
	float t[2];

	Matrix22() {
		/* Identity */
		v[1]=v[2]=t[0]=t[1]=0.0f;
		v[0]=v[3]=1.0f;
	}

	Vector2 colX() const { return Vector2(v[0], v[2]); }
	Vector2 colY() const { return Vector2(v[1], v[3]); }
	void colX(const Vector2& x) { 
		v[0] = x.x;
		v[2] = x.y;
	}
	void colY(const Vector2& y) {
		v[1] = y.x;
		v[3] = y.y;
	}

	Vector2 apply(const Vector2& a) const {
		return Vector2(
			a.x * v[0] + a.y * v[1] + t[0], 
			a.x * v[2] + a.y * v[3] + t[1]);
	}

	Matrix22 rotated(float a) const {
		return rotation(a).multiply(*this);
	}

	static Matrix22 rotation(float a){
		Matrix22 s;
		Vector2 x(a);
		s.colX(x);
		s.colY(x.ortho());
		return s;
	}

	static Matrix22 transform(Vector2 pos, Vector2 scale, float angle) {
		return Matrix22::scale(scale).rotated(angle).translated(pos);
	}

	Matrix22 transformed(Vector2 pos, Vector2 scale, float angle) {
		return transform(pos, scale, angle).multiply(*this);
	}

	static Matrix22 scale(Vector2 a) {
		Matrix22 s;
		s.v[0] = a.x;
		s.v[3] = a.y;
		return s;
	}

	Matrix22 scaled(Vector2 a) const {
		return scale(a).multiply(*this);
	}

	static Matrix22 translate(Vector2 a) {
		Matrix22 r;
		r.t[0] = a.x;
		r.t[1] = a.y;
		return r;
	}

	Matrix22 translated(Vector2 a) const {
		Matrix22 r(*this);
		r.t[0] += a.x;
		r.t[1] += a.y;
		return r;
	}

	/*
	// convert to a 4x4 matrix that is suitable for use with glLoadMatrixf()
	*/
	void transposed44Matrix(float *d) const {
		/* 
		0  1  2  3       0  4  8  12   v0  v1  0  t0
		4  5  6  7       1  5  9  13   v2  v3  0  t1
		8  9  10 11      2  6  10 14    0   0  1  0
		12 13 14 15      3  7  11 15    0   0  0  1
		*/
		for(int i=0;i<16;i++)
			d[i] = 0.0f;

		d[0] = v[0];
		d[1] = v[2];
		d[4] = v[1];
		d[5] = v[3];
		d[12] = t[0];
		d[13] = t[1];
		d[10] = d[15] = 1.0f;
	}

	Matrix22 multiply(const Matrix22& m) const {
		/*
		0 1 2   v0 v1 t0       mv0 mv1 mt0 
		3 4 5   v2 v3 t1	   mv2 mv3 mt1
		6 7 8   0  0  1          0  0  1

         ((v0, v1, t0) * (mv0, mv2, 0)),  ((v0, v1, t0) * (mv1, mv3, 0)),  ((v0, v1, t0) * (mt0, mt1, 1)
         ((v2, v3, t1) * (mv0, mv2, 0)),  ((v2, v3, t1) * (mv1, mv3, 0)),  ((v2, v3, t1) * (mt0, mt1, 1)
         0,  0,  1
		*/
		Matrix22 r;
		for(int i=0;i<2;i++) {
			for(int j=0;j<2;j++)
				r.v[i*2+j] = v[i*2+0]*m.v[0+j] + v[i*2+1]*m.v[2+j];
			r.t[i] = v[i*2+0]*m.t[0] + v[i*2+1]*m.t[1] + t[i];
		}
		return r;
	}

	Matrix22 operator*(const Matrix22& m) const {
		return multiply(m);
	}

	const Matrix22& operator*=(const Matrix22& m) {
		*this = multiply(m);
		return *this;
	}
};
