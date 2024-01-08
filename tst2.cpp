#include "ksng/quantum/qubit.h"

using namespace std;

int main() {
	braket::QV a(2);
	a.set(0, COMPLEX(1, 0));
	a.set(1, COMPLEX(0, 1));

	braket::QV b(2);
	b.set(0, COMPLEX(0, 1));
	b.set(1, COMPLEX(1, 0));

	braket::QV c = a * b;
	cout << c.ampRepr() << endl;

	sda::SDA<braket::QV> x = c.binaryDecomposition();
}