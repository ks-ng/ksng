#include "../net/pktd.h"

int main() {
	nicr::NICReader nr("eth0");
	pktd::layers::Ethernet eth;
	pktd::layers::IPv4 ipv4;
	pktd::layers::UDP udp;
	data::Bytes d;

	while (true) {
		d = nr.receiveData();
		d = eth.dissect(d);
		d = ipv4.dissect(d);
		if (ipv4.proto == 17) {
			break;
		}
	}

	cout << "IP and ethernet headers extracted" << endl;

	udp.srcp = 1234;
	udp.dstp = 5678;
	udp.length = 4;
	unsigned short ichk = ipv4.chk;

	data::Bytes ethbytes = eth.assemble();
	data::Bytes ipv4bytes = ipv4.assemble();
	data::Bytes udpbytes = udp.assemble();

	cout << "Headers assembled" << endl;

	data::Bytes pkt(ethbytes.getLength() + ipv4bytes.getLength() + udpbytes.getLength());
	ethbytes.copyTo(pkt, 0);
	ipv4bytes.copyTo(pkt, ethbytes.getLength());
	udpbytes.copyTo(pkt, ethbytes.getLength() + ipv4bytes.getLength());

	cout << "Packet built (should be 42 bytes): " << pkt.hex() << endl << "Testing vivisection ...";

	d = eth.dissect(pkt);
	d = ipv4.dissect(d);
	d = udp.dissect(d);

	unsigned short fchk = ipv4.chk;

	cout << eth.repr() << " / " << ipv4.repr() << " / " << udp.repr() << endl;
	if (ichk != fchk) {
		notif::fatal("checksum fail");
	}
}