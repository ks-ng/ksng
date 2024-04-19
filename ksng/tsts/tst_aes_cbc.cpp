#include "../crypto/ciphers/aes.h"

int main() {
	aes::AES_CBC cs;
	data::Bytes pt = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	key::Key k = cs.generateKey();
	cout << pt.hex() << endl << cs.encrypt(pt, k).hex() << endl << cs.decrypt(cs.encrypt(pt, k), k).hex() << endl;
}