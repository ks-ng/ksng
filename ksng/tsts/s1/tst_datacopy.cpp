#include "../util/data.h"

int main() {
	data::Bytes a(2);
	a.set(0, 1);
	a.set(1, 2);
	data::Bytes b(2);
	b.set(0, 3);
	b.set(1, 4);
	data::Bytes r(4);
	a.copyTo(r, 0);
	b.copyTo(r, 2);
	cout << r.hex() << endl;
}