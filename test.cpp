#include "ksng/util/data.h"

int main() {
	data::Bytes a(3);
	a.set(0, 10);
	a.set(1, 20);
	a.set(2, 30);
	cout << a.hex() << endl;
	data::Bits b = data::bytesToBits(a);
	cout << b.bin() << endl;
	cout << data::bitsToBytes(b).hex() << endl;
}