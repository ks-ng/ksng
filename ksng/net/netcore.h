#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <cstring>
#include <string>

#include "../notif.h"

using namespace std;

int inet_pton(int af, const char *src, void *dst) {
	if (af == AF_INET) {
		struct sockaddr_in sa;
		if (inet_pton(af, src, &(sa.sin_addr)) != 1) {
			return 0;  // Failed to convert
		}
		memcpy(dst, &(sa.sin_addr), sizeof(struct in_addr));
		return 1;  // Success
	} else if (af == AF_INET6) {
		struct sockaddr_in6 sa;
		if (inet_pton(af, src, &(sa.sin6_addr)) != 1) {
			return 0;  // Failed to convert
		}
		memcpy(dst, &(sa.sin6_addr), sizeof(struct in6_addr));
		return 1;  // Success
	}
	return 0;  // Invalid address family
}

struct RawPacket {
	ssize_t size;
	unsigned char data[1500];
};

class NetworkInterface {

	public:

		NetworkInterface(const std::string& interfaceName) : interfaceName(interfaceName) {
			socketDescriptor = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
			if (socketDescriptor == -1) {
				notif::fatal("Error opening raw socket");
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
		RawPacket receiveRawPacket() {
			RawPacket pkt;
			pkt.size = recv(socketDescriptor, pkt.data, sizeof(pkt.data), 0);

			if (pkt.size == -1) {
				std::cerr << "Error receiving packet" << std::endl;
			}

			return pkt;
		}

		Data receiveData() {
			RawPacket pkt = receiveRawPacket();
			Bytestring result = Bytestring(pkt.data, pkt.size);
			return result;
		}

		int getInterfaceIndex() {
			struct ifreq ifr;
			std::memset(&ifr, 0, sizeof(ifr));
			std::strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

			if (ioctl(socketDescriptor, SIOCGIFINDEX, &ifr) == -1) {
				std::cerr << "Error getting interface index" << std::endl;
				return -1;
			}

			return ifr.ifr_ifindex;
		}

	private:

		int socketDescriptor;
		std::string interfaceName;

};