#include "../crypto/hashes/marsh.h"

int main() {
	marsh::MARSH hash;
	data::Bytes pt = fileops::readFileBytes("mykey.key");
	cout << pt.hex() << endl;
	cout << "hashing ..." << endl;
	data::Bytes ct = hash.hash(pt);
	cout << ct.hex() << endl;
	cout << "loading diff ..." << endl;
	cout << (pt ^ ct).hex() << endl;
}