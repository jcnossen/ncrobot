/* 
 * File:   Matrix.h
 * Author: jcnossen
 *
 * Created on October 29, 2009, 1:07 PM
 */

#ifndef _MATRIX_H
#define	_MATRIX_H

#include "Vector.h"

namespace ncrobot {

	class Matrix {
	public:
		typedef float elem_t;

		Matrix();
		Matrix(int rows,int cols);
		Matrix(int rows,int cols, elem_t* data);
		Matrix(const Matrix& orig);
		~Matrix();

		void set(const Matrix& orig);
		Vector row(int i) const;
		Vector col(int i) const;
		void setRow(int i, const Vector& d);
		void setCol(int i, const Vector& d);
		Matrix transpose() const;
		static Matrix diag(const Vector& d);
		static Matrix ones(int n);
		static Matrix zeros(int r,int k);
		Matrix operator*(const Matrix& rhs) const;
		Matrix& operator*=(const Matrix& rhs);

		elem_t& at(int row, int col) { return d[row*k+col]; }
		const elem_t& at(int row, int col) const { return d[row*k+col]; }


	private:
		int r,k;
		elem_t *d;
	};
}

#endif	/* _MATRIX_H */

