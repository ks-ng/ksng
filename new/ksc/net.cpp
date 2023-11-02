#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>

using namespace std;

std::string charToHex(char c) {
	std::string hexString = "";
	for (int i = 0; i < 2; i++) {
		int value = (c >> (4 * (1 - i))) & 0xF;
		if (value < 10) {
			hexString += static_cast<char>('0' + value);
		} else {
			hexString += static_cast<char>('A' + (value - 10));
		}
	}
	return hexString;
}z

struct RawPacket {
	ssize_t size;
	char data[1500];
};

class NetworkInterface {

	public:

		NetworkInterface(const std::string& interfaceName) : interfaceName(interfaceName) {
			socketDescriptor = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
			if (socketDescriptor == -1) {
				std::cerr << "Error opening raw socket" << std::endl;
				// You may want to handle this error more gracefully
			}
		}

		~NetworkInterface() {
			close(socketDescriptor);
		}

		void enablePromiscuousMode() {
			struct ifreq ifr;
			std::memset(&ifr, 0, sizeof(ifr));
			std::strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

			if (ioctl(socketDescriptor, SIOCGIFFLAGS, &ifr) == -1) {
				std::cerr << "Error getting interface flags" << std::endl;
				return;
			}

			ifr.ifr_flags |= IFF_PROMISC;

			if (ioctl(socketDescriptor, SIOCSIFFLAGS, &ifr) == -1) {
				std::cerr << "Error enabling promiscuous mode" << std::endl;
			}
		}

		// Receive a packet and return it
		RawPacket receive() {
			RawPacket pkt;
			pkt.size = recv(socketDescriptor, pkt.data, sizeof(pkt.data), 0);

			if (pkt.size == -1) {
				std::cerr << "Error receiving packet" << std::endl;
			}

			return pkt;
		}

	private:

		int socketDescriptor;
		std::string interfaceName;

};

int main() {
	std::cout << "Kingslayer: Next Generation\nBuilt-in Network Packet Sniffer\nSetting up interface ...";
	NetworkInterface interface("eth0"); // Change "eth0" to your network interface name
	interface.enablePromiscuousMode();
	std::cout << "interface live.\n";

	while (true) {
		std::cout << "Waiting for packet ...";
		RawPacket pkt = interface.receive();
		if (pkt.size == -1) {
			std::cout << "Error: error occurred receiving packet. Check permissions?\n";
			continue;
		}

		std::cout << "Got packet: ";
		for (int i = 0; i < pkt.size; i++) {
			std::cout << charToHex(pkt.data[i]);
			std::cout << "-";
		}
		std::cout << "\n";
	}

	return 0;
}
