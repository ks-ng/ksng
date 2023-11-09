#include "ksng/net.cpp"

int main() {
	NetworkInterface ni("eth0");
	Ethernet eth;
	IPv4 ipv4;
	Bytestring rawData = Bytestring(1500);
	while (true) {
		ni.receiveData().copyTo(rawData);
		cout << "Received raw data: length of " << rawData.length << " bytes" << endl;
		eth.dissect(rawData, 0);
		cout << " > Ethernet packet: going to " << eth.dst.hexadecimal() << " from " << eth.src.hexadecimal() << ", ethertype " << eth.etht << endl;
		if (eth.etht == 2048) {
			ipv4.dissect(rawData, 14);
			cout << "   > IPv4 / Ethernet packet: going to " << bytestringToIPv4(ipv4.dst) << " from " << bytestringToIPv4(ipv4.src) << ", protocol " << ipv4.proto << endl;
		}
	}
}