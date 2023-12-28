#include "ksng/quantum/qubit.h"

int main() {
	qubit::QuantumRegister qr(2);
	qr.get(0).vec.set(0, COMPLEX(0.1d, 0.9d));
	qr.get(0).vec.set(1, COMPLEX(0.2d, 0.8d));
	qr.get(0).normalize();
	cout << qr.qubitRepr() << endl;
}