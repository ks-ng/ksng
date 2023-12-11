#include "ksng/crypto/otp.h"

int main() {
	OneTimePad cipher;
	Key key("key.dat");
	key.reveal();
	Bytestring data = key.access();
	cout << data.hexadecimal() << endl;
	cout << cipher.encrypt(data, key).hexadecimal() << endl;
	key.hide();
	key.access();
}