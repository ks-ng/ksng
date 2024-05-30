#include "../util/data.h"

using namespace data;

int main() {
	Bytes x = {1, 2, 3, 4, 5};
	cout << x.subbytes(2, 4).hex() << endl;
}