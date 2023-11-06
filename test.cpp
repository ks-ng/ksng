#include "ksng/net.cpp"

int main() {
	NetworkInterface ni("eth0");
	Ethernet eth;
	IPv4 ipv4;
	Bytestring rawData;
	while (true) {
		rawData = ni.receiveData();
		cout << "Received raw data: length of " << rawData.length << " bytes" << endl;
		eth.dissect(rawData, 0);
		cout << " > Ethernet packet: going to " << eth.dst.value.hexadecimal() << " from " << eth.src.value.hexadecimal() << ", ethertype " << eth.etht.value << endl;
		if (eth.etht.value == 2048) {
			ipv4.dissect(rawData, 14);
			cout << "   > IPv4 / Ethernet packet: going to " << bytestringToIPv4(ipv4.dst.value) << " from " << bytestringToIPv4(ipv4.dst.value) << ", protocol " << ipv4.proto.value << endl;
		}
	}
}