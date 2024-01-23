#include "ksng/net/conn.h"

int main() {
	conn::TCPConnection tcp("104.17.207.23", 80);
	tcp.establish();
	cout << "connection established" << endl;
	tcp.transmit(fileops::readFileBytes("t.dat"));
	cout << "message transmitted" << endl;
	data::Bytes result = tcp.receive();
	cout << "result received" << endl;
	cout << result.astext() << endl;
}