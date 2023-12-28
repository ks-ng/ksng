#include "ksng/quantum/qubit.h"

int main() {
	qubit::QuantumRegister qr(2);
	qr.getref(0).vec.set(0, COMPLEX(0.1d, 0.9d));
	qr.getref(0).vec.set(1, COMPLEX(0.2d, 0.8d));
	qr.getref(0).normalize();
	qr.applyControlledOperator(0, 1, qstd::PauliX);
	cout << qr.get(0).vec.ampRepr();
}