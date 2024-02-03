#include "../crypto/hashes/marsh.h"

int main() {
	marsh::MARSH hash;
	data::Data pt = fileops::readFileBytes(mykey.key);
	data::Data ct = hash.hash(pt);
	cout << ct.hex();
}