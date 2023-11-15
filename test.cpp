#include "ksng/net.cpp"

int main() {
	NetworkInterface ni("eth0");
	Ethernet eth;
	IPv4 ipv4;
	TCP tcp;
	UDP udp;
	Bytestring rawData = Bytestring(1500);
	while (true) {
		ni.receiveData().copyTo(rawData);
		cout << "Received raw data: length of " << rawData.length << " bytes" << endl;
		eth.dissect(rawData, 0);
		cout << " > Ethernet packet: going to " << eth.dst.hexadecimal() << " from " << eth.src.hexadecimal() << ", ethertype " << eth.etht << endl;
		if (eth.etht == 2048) {
			ipv4.dissect(rawData, 14);
			cout << "   > IPv4 / Ethernet packet: going to " << bytestringToIPv4(ipv4.dst) << " from " << bytestringToIPv4(ipv4.src) << ", protocol " << ipv4.proto << endl;
			if (ipv4.proto == 6) {
				tcp.dissect(rawData, ipv4.ihl * 4);
				cout << "     > TCP / IP packet: going to port " << (unsigned int)(tcp.dstp) << " from port " << (unsigned int)(tcp.srcp) << endl;
			}
			if (ipv4.proto == 17) {
				udp.dissect(rawData, ipv4.ihl * 4);
				cout << "     > UDP / IP packet: going to port " << (unsigned int)(udp.dstp) << " from port " << (unsigned int)(udp.srcp) << endl;
			}
		}
	}
}