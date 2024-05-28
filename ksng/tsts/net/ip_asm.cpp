#include "../../net/pktd.h"

int main() {
	pktd::PacketDissector pd("eth0");
	pktd::Packet pkt = pd.receivePacket();
	pkt.ipv4.report();
	pktd::layers::IPv4 nipv4;
	nipv4.dissect(pkt.ipv4.assemble());
	nipv4.report();
	cout << (nipv4.assemble() ^ pkt.ipv4.assemble()).hex() << endl;
}