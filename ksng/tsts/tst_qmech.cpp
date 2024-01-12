#include "../quantum/qcomp.h"

using namespace std;

int main() {
	qmech::QuantumVector a1(2);
	a1.set(0, AMP0);
	a1.set(1, AMP1);
	qmech::QuantumVector a2(2);
	a2.set(0, AMP1);
	a2.set(1, AMP0);
	qmech::QuantumVector a3(2);
	a3.set(0, AMP1);
	a3.set(1, AMP0);
	qmech::QuantumVector a4(2);
	a4.set(0, AMP0);
	a4.set(1, AMP1);
	qmech::QuantumVector a5(2);
	a5.set(0, AMP0);
	a5.set(1, AMP1);
	qmech::QuantumVector a6(2);
	a6.set(0, AMP1);
	a6.set(1, AMP0);
	qmech::QuantumVector z = a1 * a2 * a3 * a4 * a5 * a6;
	cout << "a1: " << a1.repr() << endl;
	cout << "a2: " << a2.repr() << endl;
	cout << "a3: " << a3.repr() << endl;
	cout << "a4: " << a4.repr() << endl;
	cout << "a5: " << a5.repr() << endl;
	cout << "a6: " << a6.repr() << endl;
	sda::SDA<qmech::QuantumVector> zd = z.binaryDecomp();
	cout << "zd size: " << zd.getLength() << endl;
	cout << "zd.get(0): " << zd.get(0).repr() << endl;
	cout << "zd.get(1): " << zd.get(1).repr() << endl;
	cout << "zd.get(2): " << zd.get(2).repr() << endl;
	cout << "zd.get(3): " << zd.get(3).repr() << endl;
	cout << "zd.get(4): " << zd.get(4).repr() << endl;
	cout << "zd.get(5): " << zd.get(5).repr() << endl;

	cout << endl;

	qcomp::Q qubit(1);
	cout << qubit.repr() << endl;
}