#include "ksng/utils.cpp"

int main() {
	unsigned char raw1[3] = {1, 2, 3};
	unsigned char raw2[3] = {4, 5, 6};
	Bytestring d1(raw1, 3);
	Bytestring d2(raw2, 3);
	d2 = d1.concatenate(d2);
	cout << d2.hexadecimal() << endl;
	cout << d2.substring(1, 3).hexadecimal() << endl;
	Bytestring x(0);
	x = x.concatenate(d2);
	cout << x.hexadecimal() << endl;
	return 0;
}