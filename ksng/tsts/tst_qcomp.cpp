#include "../quantum/qcomp.h"

using namespace std;

int main() {
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