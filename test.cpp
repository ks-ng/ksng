#include "ksng/net.cpp"

int main() {
	NetworkInterface ni("eth0");
	Ethernet eth;
	IPv4 ipv4;
	Bytestring rawData;
	while (true) {
		rawData = ni.receiveData();
		cout << "Got raw data " << rawData.hexadecimal() << endl;
		eth.dissect(rawData, 0);
		cout << " > Got Ethernet packet going to " << eth.dst.value.hexadecimal() << " from " << eth.src.value.hexadecimal() << endl;
		if (eth.etht.value == 2048) {
			ipv4.dissect(rawData, 14);
			cout << " > IP packet going to " << ipv4.dst.value.hexadecimal() << endl;
		}
	}
}