#include "ksng/crypto.cpp"

int main() {
	impl::OneTimePad cipher;
	unsigned char raw[5] = {1, 2, 3, 4, 5};
	Bytestring rkd(raw, 5);
	basic::Key key(&rkd);
	cipher.encryptFile("t.dat", key);
}