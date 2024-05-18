#include "../../net/pktd.h"

int main() {
	pktd::PacketDissector pd("eth0");
	pktd::Packet pkt = pd.receivePacket();
	cout << pkt.receptionData.hex() << endl;
	cout << pkt.assemble().hex() << endl;
}