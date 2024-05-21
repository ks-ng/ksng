#include "../../util/dstruct/sda.h"

int main() {
	sda::Array<int> x = {1, 2, 3};
	cout << x[1] << endl;
	x[1] = 0;
	cout << x[1] << endl;
}