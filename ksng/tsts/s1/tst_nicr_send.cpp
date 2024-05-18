#include "../net/nicr.h"

using namespace std;

int main() {
	nicr::NICR nr("eth0");
	cout << "awaiting input, ping this machine to move on" << endl;
	data::Bytes d = nr.receiveData();
	cout << "packet data: " << d.hex() << endl;
	cout << "sending packet ..." << endl;
	nr.sendData(d);
	cout << "done." << endl;
}