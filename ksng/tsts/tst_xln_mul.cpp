#include "../crypto/core/xln.h"

using namespace xln;

int main() {
	XLN a(8);
	XLN b(8);
	XLN c(8);
	XLN d(8);
	XLN e(8);
	XLN f(8);
	a.set(0, 1);
	a.set(2, 1);
	b.set(1, 1);
	c.set(2, 1);
	d.set(1, 1);
	d.set(2, 1);
	e.set(7, 1);
	f.set(3, 1);
	cout << "a: " << a.num() << endl;
	cout << "b: " << b.num() << endl;
	cout << "c: " << c.num() << endl;
	cout << "d: " << d.num() << endl;
	cout << "e: " << e.num() << endl;
	cout << "f: " << f.num() << endl;
	cout << "a * b: " << (a * b).num() << endl;
	cout << "a * c: " << (a * c).num() << endl;
	cout << "a * d: " << (a * d).num() << endl;
	cout << "e * f: " << (e * f).num() << endl;
}