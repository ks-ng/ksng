#include "../net/conn.h"

int main() {
	conn::UDPConnection udp("127.0.0.1", 7777);
	data::Bytes msg(3);
	msg.set(0, 1);
	msg.set(1, 2);
	msg.set(2, 3);
	udp.transmit(msg);
}