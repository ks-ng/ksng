#include "../net/pktd.h"

int main() {
	pktd::layers::Ethernet eth("12:34:56:78:90:00", "00:09:87:65:43:21", 1234);
	cout << eth.repr() << endl;
	data::Bytes assembly = eth.assemble();
	cout << "assembly hex: " << assembly.hex() << endl;
	cout << "waiting a sec" << endl;
	//cout << "dissection: " << eth.dissect(assembly).repr() << endl;
	eth.dst = pktd::layers::bytesToEthaddr(assembly.subbytes(0, 6));
	cout << "dst done ...";
	eth.src = pktd::layers::bytesToEthaddr(assembly.subbytes(6, 12));
	cout << "src done ...";
	eth.etht = (assembly.get(12) * 256) + assembly.get(13);
	cout << "etht done." << endl;
}