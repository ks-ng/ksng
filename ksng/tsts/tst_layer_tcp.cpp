#include "../net/pktd.h"

int main() {
	nicr::NICReader nr("eth0");
	pktd::layers::Ethernet eth;
	pktd::layers::IPv4 ipv4;
	pktd::layers::TCP tcp;

	while (true) {
		data::Bytes d = nr.receiveData();
		d = eth.dissect(d);
		d = ipv4.dissect(d);
		if (ipv4.proto == 6) {
			d = tcp.dissect(d);
			cout << eth.repr() << " / " << ipv4.repr() << " / " << tcp.repr() << endl;
			break;
		}
	}
}