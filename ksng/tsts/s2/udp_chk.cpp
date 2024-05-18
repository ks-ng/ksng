#include "../../net/pktd.h"

int main() {
	cout << "loading pktd ..." << endl;
	pktd::PacketDissector pd("eth0");
	cout << "done, getting packet ...";
	pktd::Packet pkt;
	while (!pkt.udp.dissected) {
		pkt = pd.receivePacket();
	}
	cout << "done." << endl;
	cout << "current UDP checksum: " << pkt.udp.chk << endl;
	cout << "recalculating checksum ...";
	pkt.fixUDP(true);
	cout << "done.\nnew UDP checksum: " << pkt.udp.chk << endl;
}