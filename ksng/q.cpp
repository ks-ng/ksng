#pragma once

#include <complex>
#include <cmath>

using namespace std;

#define COMPLEX complex<double>

class QVec {

	public:

		int size;
		COMPLEX* elements;

		QVec(int size): size(size), elements(new COMPLEX[size]) {}
		QVec(int size, COMPLEX data[]): size(size), elements(data) {}

		COMPLEX operator[](int index) {
			return elements[index];
		}

};

class QMatrix {

	public:

		int size;
		QVec* rows;

		QMatrix(int size): size(size), rows(new QVec[size]) {
			for (int i = 0; i < size; i++) {
				rows[i] = QVec(size);
			}
		}
		
		QMatrix(int size, QVec data[]): size(size), rows(data) {}

		QVec operator[](int index) {
			return rows[index]
		}

		void make(QVec a, QVec b) {
			for (int i = 0;)
		}

};