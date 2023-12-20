#include "ksng/util/datacls.h"

using namespace datacls;

int main() {
	Data d(10, bytes, false);
	d.setBit(0, 1);
	d.setBit(1, 1);
	d.setBit(2, 1);
	d.setBit(3, 1);
	d.setByte(1, 2);
	d.setByte(2, 3);
	Data d2 = d.sub(1, 3);
	cout << d.hex() << " " << d2.hex() << endl;

	Data a(3);
	Data b(3);

	a.setByte(0, 255);
	a.setByte(1, 0);
	a.setByte(2, 255);

	b.setByte(0, 0);
	b.setByte(1, 255);
	b.setByte(2, 255);

	cout << (a + b).hex() << endl;
}