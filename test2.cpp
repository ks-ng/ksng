#include "ksng/utils.cpp"

int main() {
	unsigned char x[6] = {1, 2, 3, 4, 5, 69};

	Bytestring bs = Bytestring(x, 6);
	cout << bs.hexadecimal() << endl;
	cout << bs.data[5] << endl;
	cout << bs.substring(1, 3).hexadecimal() << endl;
}