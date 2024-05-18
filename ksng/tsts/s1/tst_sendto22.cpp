#include "../net/conn.h"

int main() {
	conn::UDPConnection udp("10.0.0.22", 1024);
	udp.establish();
	udp.transmit(data::Bytes({0, 1, 2, 3, 4, 5, 6, 7}));
}