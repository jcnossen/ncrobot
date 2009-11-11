/* 
 * File:   Matrix.cpp
 * Author: jcnossen
 * 
 * Created on October 29, 2009, 1:07 PM
 */

#include "StdIncl.h"
#include "Matrix.h"

namespace ncrobot {

	Matrix::Matrix() {
		d=0;
		r=k=0;
	}

	Matrix::Matrix(int r,int k) {
		this->r=r;
		this->k=k;
		this->d=new elem_t[r*k];
		memset(d,0,sizeof(elem_t)*r*k);
	}

	Matrix::Matrix(const Matrix& orig) {
		d=0;
		set(orig);
	}

	void Matrix::set(const Matrix& orig) {
		if (d) delete[] d;
		r=orig.r;
		k=orig.k;
		d=new elem_t[r*k];
		elem_t* dst=d;
		elem_t* src=orig.d;
		for(int y=0;y<r;y++)
			for(int x=0;x<k;x++)
				*(dst++) = *(src++);
	}

	Matrix::~Matrix() {
		if(d) delete[] d;
	}


	Vector Matrix::row(int i) const {
		Vector v(k);
		for(int x=0;x<k;x++)
			v[x]=at(i,x);
		return v;
	}

	Vector Matrix::col(int i) const {
		Vector v(r);
		for(int y=0;y<r;y++)
			v[y]=at(y,i);
		return v;
	}

	void Matrix::setRow(int i, const Vector& d) {
		assert(d.n==k);
		for(int x=0;x<k;x++)
			at(i,x)=d[x];
	}
	void Matrix::setCol(int i, const Vector& d) {
		assert(d.n==r);
		for(int x=0;x<r;x++)
			at(x,i)=d[x];
	}

	Matrix Matrix::transpose() const {
		Matrix tr (k,r);
		for(int row=0;row<r;row++)
			for(int col=0;col<k;col++)
				tr.at(col,row)=at(row,col);
		return tr;
	}

	Matrix Matrix::diag(const Vector& d) {
		Matrix r(d.n,d.n);
		for(int x=0;x<d.n;x++)
			r.at(x,x)=d[x];
		return r;
	}

//	static Matrix Matrix::ones(int n);
	Matrix Matrix::zeros(int r, int k) { return Matrix(r,k); }


	/*
	Matrix Matrix::operator*(const Matrix& rhs) const {
		Matrix m;
		return m;
	}
	Matrix& Matrix::operator*=(const Matrix& rhs) {
		return *this;
	}*/
}
