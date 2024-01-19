#include "../crypto/ciphers/vox.h"

int main() {
	VOX cipher;
	key::Key k = key::Key::generate();
	data::Bytes plaintext = csprng::bytes(128);
	data::Bytes ciphertext = cipher.encryptBlock(plaintext, k);
	data::Bytes decrypted = cipher.decryptBlock(ciphertext, k);
	cout << "plaintext: " << plaintext.hex() << endl;
	cout << "ciphertext: " << ciphertext.hex() << endl;
	cout << "decrypted: " << decrypted.hex() << endl;
	if (plaintext == decrypted) {
		cout << "plaintext matches decryption" << endl;
	} else {
		cout << "plaintext does not match decryption" << endl;
	}
}