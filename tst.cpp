#include "ksng/crypto/core.cpp"

int main() {
	Key key("key.dat");
	key.reveal();
	Bytestring data = key.access();
	cout << data.hexadecimal();
	key.hide();
	key.access();
}