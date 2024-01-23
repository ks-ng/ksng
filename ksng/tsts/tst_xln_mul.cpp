#include "../crypto/core/xln.h"

using namespace xln;

int main() {
	XLN a(8);
	XLN b(8);
	XLN c(8);
	XLN d(8);
	a.set(0, 1);
	a.set(2, 1);
	b.set(1, 1);
	c.set(2, 1);
	d.set(1, 1);
	d.set(2, 1);
	cout << "a: " << a.bin() << endl;
	cout << "b: " << b.bin() << endl;
	cout << "c: " << c.bin() << endl;
	cout << "d: " << d.bin() << endl;
	cout << "a * b: " << (a * b).bin() << endl;
	cout << "a * c: " << (a * c).bin() << endl;
	cout << "a * d: " << (a * d).bin() << endl;
}