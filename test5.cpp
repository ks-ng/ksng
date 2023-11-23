#include "ksng/net.cpp"

int main() {
	NetworkInterface ni("eth0");
	cout << ni.getInterfaceIndex();
	cout << ni.receiveData().hexadecimal();
}