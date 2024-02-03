#include "../util/xln.h"

using namespace xln;

int main() {
	XLN a(8);
	XLN b(8);
	XLN c(6);
	a.set(7, 1);
	b.set(4, 1);
	c.set(5, 1);
	cout << (a > b) << (a > c) << (b > a) << (b > c) << (c > b); // 11001
}