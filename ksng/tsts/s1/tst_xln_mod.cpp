#include "../util/xln.h"

using namespace xln;

int main() {
	cout << "building a" << endl;
	XLN a(8);
	a.set(7, 1);
	cout << "building b" << endl;
	XLN b(8);
	b.set(6, 1);
	b.set(5, 1);
	b.set(2, 1);
	cout << "taking mod" << endl;
	XLN c = a % b;
	cout << a.num() << endl << b.num() << endl << c.num() << endl;
}