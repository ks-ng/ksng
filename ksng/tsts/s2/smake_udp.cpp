#include "../../net/pktd.h"

using namespace pktd;

int main() {
	PacketDissector pd("eth0");
	Packet pkt = pd.receivePacket();
	while (!pkt.udp.dissected) { pkt = pd.receivePacket(); }
	data::Bytes d = pkt.assemble();
	cout << "found udp packet: " << pkt.repr() << endl;
	cout << "assembled udp packet: " << d.hex() << endl;
	cout << "running smake ...";
	data::Bytes npktd = smake::smakeUDP(pkt.eth.src, pkt.eth.dst, pkt.ipv4.src, pkt.ipv4.dst, pkt.udp.srcp, pkt.udp.dstp, pkt.payload);
	cout << "done.\nsmade packet: " << npktd.hex() << endl;
	cout << "dissecting ...";
	Packet npkt = pd.dissectPacket(npktd);
	npkt.udp.chk = pkt.udp.chk;
	cout << "done, new packet: " << npkt.repr() << endl;
	cout << "diff: " << (d ^ npktd).hex() << endl;
	cout << "ipv4 checksum compare: " << pkt.ipv4.chk - npkt.ipv4.chk << endl;
	cout << "ipv4 initial id: " << pkt.ipv4.ipid << endl;
	cout << "ipv4 initial frags and flags: " << pkt.ipv4.fragoff << " " << pkt.ipv4.flags << endl;
	pkt.ipv4.report();
	npkt.ipv4.report();
}