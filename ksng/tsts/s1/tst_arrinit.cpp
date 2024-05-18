#include "../util/data.h"

int main() {
	sda::SDA<int> x = {1, 2, 3, 4, 5};
	data::Bytes y = {1, 2, 8, 9, 100};
	cout << x.repr() << endl;
	cout << y.hex();
}