#include "../crypto/core/xln.h"

using namespace xln;

int main() {
	cout << "computing 5 ^ 9 mod 256" << endl;
	XLN a(8);
	a.set(0, 1);
	a.set(2, 1);
	XLN b(8);
	b.set(0, 1);
	b.set(3, 1);
	cout << "a: " << a.bin() << endl;
	cout << "b: " << b.bin() << endl;
	cout << "a ^ b: " << (a.exponentiate(b)).bin() << endl;
}