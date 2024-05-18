#include "../net/pktd.h"

int main() {
	nicr::NICReader nr("eth0");
	pktd::layers::Ethernet eth;
	pktd::layers::IPv4 ipv4;
	data::Bytes d = nr.receiveData();
	cout << d.hex() << endl;
	data::Bytes r = eth.dissect(d);
	r = ipv4.dissect(r);
	cout << eth.repr() << " " << ipv4.repr() << endl;
}