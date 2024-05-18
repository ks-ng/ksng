#include "../net/pktd.h"

data::Bytes signal = {0, 1, 2, 3};

int main() {
	pktd::PacketDissector pd("eth0");
	pktd::Packet pkt;
	while (true) {
		pkt = pd.receivePacket();
		cout << pkt.payload.hex() << endl;
		if (pkt.udp.dissected && (pkt.payload == signal)) {
			cout << "Duplicating UDP" << endl << endl;
			pd.nr.sendData(pkt.receptionData + data::Bytes({1, 2, 3, 4, 5, 6, 7, 8}));
		}
	}
}