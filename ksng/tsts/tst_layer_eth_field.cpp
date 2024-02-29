#include "../net/pktd.h"

int main() {
	nicr::NICReader nr("eth0");
	pktd::layers::Ethernet eth;
	data::Bytes d = nr.receiveData();
	cout << "received data: " << d.hex() << endl;
	data::Bytes r = eth.dissect(d);
	cout << "dissected eth: " << eth.repr() << endl;
	cout << "remainder: " << r.hex() << endl;
}