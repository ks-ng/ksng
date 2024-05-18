#include "../util/xln.h"

using namespace xln;

int main() {
	XLN a(64);
	XLN b(64);
	a.set(0, 1);
	a.set(62, 1);
	a.set(26, 1);
	cout << "a: " << a.num() << endl;
	b.set(62, 1);
	b.set(26, 1);
	b.set(27, 1);
	cout << "b: " << b.num() << endl;
	XLN c = a + b;
	cout << "c: " << c.num() << endl;
	cout << "c >> 8: " << (c >> 8).num() << endl;
	cout << "c >> 0: " << (c >> 0).num() << endl;
}