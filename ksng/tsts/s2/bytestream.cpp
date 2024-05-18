#include "../../util/data.h"

int main() {
	data::Bytestream bs;
	bs << data::Bytes({1, 2, 3, 4});
	bs << data::Bytes({5, 6, 7, 8});
	cout << bs.bytes().hex() << endl;
}