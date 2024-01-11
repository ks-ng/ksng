#include "ksng/quantum/qcomp.h"

using namespace std;

int main() {
	qmech::QuantumVector x(2);
	x.set(0, AMP0);
	x.set(1, AMP1);
	qmech::QuantumVector y(2);
	y.set(0, AMP1);
	y.set(1, AMP0);
	qmech::QuantumVector z = x * y;
	cout << "x: " << x.repr() << ", y: " << y.repr() << ", z: " << z.repr() << endl;
	sda::SDA<qmech::QuantumVector> zd = z.binaryDecomp();
	cout << "zd size: " << zd.getLength() << endl;
	cout << "zd.get(0): " << zd.get(0).repr() << endl;
	cout << "zd.get(1): " << zd.get(1).repr() << endl << endl;

	qcomp::Q qubit(1);
	cout << qubit.repr() << endl;
}