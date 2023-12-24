#include "ksng/util/data.h"

using namespace data;

int main() {
	Bytes a(5);
	for (int i = 0; i < 5; i++) {
		a.set(i, i);
	}
	cout << a.hex() << endl << a.subbytes(1, 4).hex() << endl;
}