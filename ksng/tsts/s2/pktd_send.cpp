#include "../../net/pktd.h"

int main() {
	pktd::PacketDissector pd("eth0");
	pktd::Packet mypkt;
	mypkt.eth.etht = 2048;
	mypkt.eth.src = "00:15:5D:85:18:2C";
	mypkt.eth.dst = "01:00:5E:7F:FF:FA";
	
}