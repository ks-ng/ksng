#include "../crypto/ciphers/vox.h"
#define SIZE 192

int main() {
	vox::VOX cipher;
	key::Key k = cipher.generateKey();
	data::Bytes plaintext = csprng::bytes(SIZE);
	cout << "plaintext: " << plaintext.hex() << endl;
	data::Bytes ciphertext = cipher.encrypt(plaintext, k);
	cout << "ciphertext: " << ciphertext.hex() << endl;
	data::Bytes decrypted = cipher.decrypt(ciphertext, k);
	cout << "decrypted: " << decrypted.hex() << endl;
	if (decrypted.subbytes(0, SIZE) == plaintext) {
		cout << "stripped decryption equals plaintext" << endl;
	} else {
		cout << "stripped decryption does not equal plaintext" << endl;
	}
}