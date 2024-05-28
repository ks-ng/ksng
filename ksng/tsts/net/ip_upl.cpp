#include "../../net/uplink.h"
#include "../../net/smake.h"

int main() {
	pktd::PacketDissector pd("eth0");
	uplink::IPv4Uplink upl;
	
	pktd::Packet pkt = smake::smakeUDP("10.0.0.222", "10.0.0.70", 7777, 7777, data::Bytes({1, 2}));

	pkt.ipv4.report();

	upl.transmit(pkt);
}