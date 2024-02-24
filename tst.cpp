#include "ksng/util/dstruct/sda.h"

int main() {
	sda::SDA<int> x = {1, 2, 3};
	cout << x.get(0) << x.get(1) << x.get(2) << endl;
}