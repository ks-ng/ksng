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

#include "../util/notif.h"
#include "../util/datacls.h"

using namespace std;

namespace netcore {

	struct RawPacket {
		ssize_t size;
		unsigned char data[1500];
	};

	class NetworkInterface {

		public:

			NetworkInterface(const std::string& interfaceName) : interfaceName(interfaceName) {
				socketDescriptor = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
				if (socketDescriptor == -1) {
					notif::fatal("Error opening raw socket; possible permissions error");
				}
				notif::success("Successfully opened raw socket.");
			}

			~NetworkInterface() {
				close(socketDescriptor);
			}

			void enablePromiscuousMode() {
				struct ifreq ifr;
				memset(&ifr, 0, sizeof(ifr));
				strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

				if (ioctl(socketDescriptor, SIOCGIFFLAGS, &ifr) == -1) {
					notif::fatal("Error getting interface flags");
					return;
				}

				ifr.ifr_flags |= IFF_PROMISC;

				if (ioctl(socketDescriptor, SIOCSIFFLAGS, &ifr) == -1) {
					notif::fatal("Error enabling promiscuous mode");
				}

				notif::success("Successfully enabled promiscuous mode.");
			}

			// Receive a packet and return it
			RawPacket receiveRawPacket() {
				RawPacket pkt;
				pkt.size = recv(socketDescriptor, pkt.data, sizeof(pkt.data), 0);

				if (pkt.size == -1) {
					notif::fatal("Error receiving packet");
				}

				return pkt;
			}

			datacls::Data receiveData() {
				RawPacket pkt = receiveRawPacket();
				datacls::Data result(pkt.size);
				for (int i = 0; i < pkt.size; i++) {
					result.setByte(i, pkt.data[i]);
				}
				return result;
			}

			int getInterfaceIndex() {
				struct ifreq ifr;
				memset(&ifr, 0, sizeof(ifr));
				strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

				if (ioctl(socketDescriptor, SIOCGIFINDEX, &ifr) == -1) {
					notif::fatal("Error getting interface index");
					return -1;
				}

				return ifr.ifr_ifindex;
			}

		private:

			int socketDescriptor;
			string interfaceName;

	};

};