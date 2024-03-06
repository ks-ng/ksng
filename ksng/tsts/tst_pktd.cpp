#include "../net/pktd.h"

int main() {
	pktd::PacketDissector pd("eth0");
	cout << pd.receivePacket().repr();
}