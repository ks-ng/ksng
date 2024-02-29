#include "../net/pktd.h"

int main() {
	nicr::NICReader nr("eth0");
	pktd::layers::Ethernet eth;
	pktd::layers::IPv4 ipv4;
	data::Bytes r = eth.dissect(nr.receiveData());
	r = ipv4.dissect(r);
	cout << eth.repr() << " " << ipv4.repr() << endl;
}