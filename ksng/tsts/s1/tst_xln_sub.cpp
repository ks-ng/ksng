#include "../util/xln.h"

int main() {
	xln::XLN a(8);
	a.set(0, 1);
	a.set(1, 1);
	a.set(4, 1);
	a.set(7, 1);
	cout << a.num() << endl;

	xln::XLN b(8);
	b.set(1, 1);
	b.set(2, 1);
	cout << b.num() << endl;

	cout << (a - b).num() << endl;
}