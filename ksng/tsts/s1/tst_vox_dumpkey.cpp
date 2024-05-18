#include "../crypto/ciphers/vox.h"

int main() {
	vox::VOX cipher;
	key::Key k = cipher.generateKey();
	fileops::writeFile("keydata.dat", k.getBytes());
	cout << "done" << endl;
	cout << "raw key data: " << k.getBytes().hex() <<endl;
}