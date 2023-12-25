#include "ksng/quantum/braket.h"

int main() {
	braket::QV qv(3);
	qv.set(1, COMPLEX(0.2d, 0.8d));
	cout << qv.ampRepr() << endl << qv.probRepr();
}