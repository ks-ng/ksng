#include "../../net.cpp"

int main() {
	NetworkInterface ni("eth0");
	Ethernet eth;
	ARP arp;
	IPv4 ipv4;
	TCP tcp;
	UDP udp;
	ICMP icmp;
	Bytestring rawData = Bytestring(1500);
	while (true) {
		ni.receiveData().copyTo(rawData);
		cout << "Received raw data: length of " << rawData.length << " bytes" << endl;
		eth.dissect(rawData, 0);
		cout << " > Ethernet packet: going to " << eth.dst.hexadecimal() << " from " << eth.src.hexadecimal() << ", ethertype " << eth.etht << endl;
		
		if (eth.etht == 2054) {
			arp.dissect(rawData, 14);
			cout << " > ARP / Ethernet packet: ";
			if (arp.op == 1) {
				cout << arp.sha.hexadecimal() << " (" << bytestringToIPv4(arp.spa) << ") asking who has " << bytestringToIPv4(arp.tpa) << endl;
			} else if (arp.op == 2) {
				cout << "replying to " << arp.tha.hexadecimal() << " (" << bytestringToIPv4(arp.tpa) << "), " << arp.sha.hexadecimal() << " is at " << bytestringToIPv4(arp.spa) << endl;
			}
		}
		if (eth.etht == 2048) {
			ipv4.dissect(rawData, 14);
			cout << " > IPv4 / Ethernet packet: going to " << bytestringToIPv4(ipv4.dst) << " from " << bytestringToIPv4(ipv4.src) << ", protocol " << ipv4.proto << endl;
			
			if (ipv4.proto == 1) {
				icmp.dissect(rawData, ipv4.ihl * 4);
				cout << " > ICMP / IP packet: type " << (int)(icmp.type) << ", code " << (int)(icmp.code) << ", data " << icmp.data << endl;
			}
			if (ipv4.proto == 6) {
				tcp.dissect(rawData, ipv4.ihl * 4);
				cout << " > TCP / IP packet: going to port " << (unsigned int)(tcp.dstp) << " from port " << (unsigned int)(tcp.srcp) << endl;
			}
			if (ipv4.proto == 17) {
				udp.dissect(rawData, ipv4.ihl * 4);
				cout << " > UDP / IP packet: going to port " << (unsigned int)(udp.dstp) << " from port " << (unsigned int)(udp.srcp) << endl;
			}
		}
		if (eth.etht == 34525)
		cout << endl;
	}
}