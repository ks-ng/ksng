#include "../net/pktd.h"

int main() {
	cout << "loading pktd ..." << endl;
	pktd::PacketDissector pd("eth0");
	cout << "done, getting packet ...";
	pktd::Packet pkt;
	while (!pkt.ipv4.dissected) {
		pkt = pd.receivePacket();
	}
	cout << "done." << endl;
	cout << "packet IPv4 header: " << pkt.ipv4.repr() << " - checksum: " << (unsigned int)(pkt.ipv4.chk) << endl;
	cout << "reassembling IPv4 header from parameters ..." << endl;
	data::Bytes reas = pkt.ipv4.assemble();
	cout << "  reassembled header raw data: " << reas.hex() << endl; 
	pktd::layers::IPv4 v4;
	v4.dissect(reas);
	cout << "reassembled IPv4 header: " << v4.repr() << " - checksum: " << (unsigned int)(v4.chk) << endl;
}