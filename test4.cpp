#include "ksng/crypto.cpp"

class MyCipher: public basic::Cipher {

	public:

		Bytestring encrypt(Bytestring plaintext, basic::Key key) {
			key.reveal();
			Bytestring result = Bytestring(plaintext, key.accessData());
			key.hide();
			return result;
		}

		Bytestring decrypt(Bytestring ciphertext, basic::Key key) {
			key.reveal();
			Bytestring result = Bytestring(ciphertext, key.accessData());
			key.hide();
			return result;
		}

};

int main() {
	MyCipher cipher;
	unsigned char raw[5] = {1, 2, 3, 4, 5};
	Bytestring rkd(raw, 5);
	basic::Key key(&rkd);
	cipher.encryptFile("t.dat", key);
}