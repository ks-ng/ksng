#include "../util/data.h"

int main() {
	int x = 1000;
	data::Bytes d(4);
	d.loadInt(x, 0);
	cout << d.hex() << endl;
}