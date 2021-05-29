#pragma once
#include <cmath>
#include <windows.h>

//класс для работы с матрицами аффинных преобразований
//используются однородные координаты
class Matrix2D {

public:
	double x[3][3];

	Matrix2D();                               //матрица единичная

	void rotate(double fi);                      //матрица поворота
	void displa(double kx, double ky);           //матрица растяжения
	void transl(double tx, double ty);          //матрица трансляции

												//перемножение матриц
	friend Matrix2D operator * (const Matrix2D&, const Matrix2D&);

};

Matrix2D::Matrix2D() {
	x[0][0] = 1; x[0][1] = 0; x[0][2] = 0;
	x[1][0] = 0; x[1][1] = 1; x[1][2] = 0;
	x[2][0] = 0; x[2][1] = 0; x[2][2] = 1;
}

void Matrix2D::rotate(double fi) {
	double cosfi = cos(fi), sinfi = sin(fi);
	x[0][0] = cosfi; x[0][1] = -sinfi; x[0][2] = 0;
	x[1][0] = sinfi; x[1][1] = cosfi; x[1][2] = 0;
	x[2][0] = 0; x[2][1] = 0; x[2][2] = 1;
}

void Matrix2D::displa(double kx, double ky) {
	x[0][0] = kx; x[0][1] = 0; x[0][2] = 0;
	x[1][0] = 0; x[1][1] = ky; x[1][2] = 0;
	x[2][0] = 0; x[2][1] = 0; x[2][2] = 1;
}

void Matrix2D::transl(double tx, double ty) {
	x[0][0] = 1; x[0][1] = 0; x[0][2] = tx;
	x[1][0] = 0; x[1][1] = 1; x[1][2] = ty;
	x[2][0] = 0; x[2][1] = 0; x[2][2] = 1;
}

Matrix2D operator * (const Matrix2D& a, const Matrix2D& b) {
	Matrix2D c;
	double s;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			s = 0;
			for (int n = 0; n < 3; n++)
				s += a.x[i][n] * b.x[n][j];
			c.x[i][j] = s;
		}
	return c;
}

//структура для описания точек фигуры
struct Point {
	double x;
	double y;
};


//аффинное преобразование координат точки P с помощью
//матрицы A
Point affine(Matrix2D A, Point P) {
	Point Pp;
	Pp.x = A.x[0][0] * P.x + A.x[0][1] * P.y + A.x[0][2] * 1;
	Pp.y = A.x[1][0] * P.x + A.x[1][1] * P.y + A.x[1][2] * 1;
	return Pp;
}

POINT affine(Matrix2D A, POINT P) {
	POINT Pp;
	Pp.x = A.x[0][0] * P.x + A.x[0][1] * P.y + A.x[0][2] * 1;
	Pp.y = A.x[1][0] * P.x + A.x[1][1] * P.y + A.x[1][2] * 1;
	return Pp;
}



