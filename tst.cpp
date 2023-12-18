#include "ksng/util.h"

int main() {
	data::Data d(10, data::bytes, false);
	cout << d.hex() << endl;
}