#include "qcomp.h"

using namespace std;

int main() {
	cout << "Quantum computing demonstration:" << endl;
	cout << " 1. Initialize three qubits using the qcomp::QuantumRegister class" << endl;
	cout << " 2. Apply a Pauli X gate to the second qubit" << endl;
	cout << " 3. Apply a Pauli X gate on the third qubit controlled on the second (no effect, qubit 3 is 100% 0)" << endl;
	cout << " 4. Apply a Pauli X gate on the third qubit controlled on the first (works, since first qubit is 100% 1)" << endl;

	qcomp::QR qr(3); // quantum nybble (qunybble?)
	for (int i = 0; i < 3; i++) {
		qr.get(i).initialize(1, 0);
	}
	cout << "initialized: " << qr.qubitRepr() << endl;
	qr.applyOperator(1, qstd::PauliX());
	cout << "PauliX on 1: " << qr.qubitRepr() << endl;
	qr.applyControlledOperator(2, 1, controlled(qstd::PauliX()));
	cout << "CPX on 2 C1: " << qr.qubitRepr() << endl;
	qr.applyControlledOperator(2, 0, controlled(qstd::PauliX()));
	cout << "CPX on 2 C0: " << qr.qubitRepr() << endl;
}