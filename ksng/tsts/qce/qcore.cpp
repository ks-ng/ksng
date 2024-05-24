#include "../../qce/qcore.h"

int main() {
	qcore::QS a = {0, 1};
	qcore::QS b = {1, 0};
	qcore::QuantumState c = a * b;
	cout << a.binStates() << endl;
	cout << b.binStates() << endl;
	cout << c.binStates() << endl;
	sda::Array<qcore::QuantumState> d = c.extractQuantumData();
	cout << d.get(0).binStates() << endl;
	cout << d.get(1).binStates() << endl;
}