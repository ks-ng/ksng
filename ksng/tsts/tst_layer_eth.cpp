#include "../net/pktd.h"

int main() {
	pktd::layers::Ethernet eth("12:34:56:78:90:00", "00:09:87:65:43:21", 1234);
	cout << "ethernet l: " << eth.repr() << endl;
	data::Bytes assembly = eth.assemble();
	cout << "assembly hex: " << assembly.hex() << endl;
	eth.dissect(assembly);
	cout << "dissection: " << eth.repr() << endl;
	string newsrc = "11:22:33:44:55:66";
	eth.src = newsrc;
	cout << "new src should be " << newsrc << ": " << eth.repr() << endl;
}