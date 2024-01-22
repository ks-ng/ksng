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
#include "../util/data.h"

using namespace std;

namespace nicr {

	struct RawPacket {
		ssize_t size;
		unsigned char data[1500];
	};

	class NICReader {

		private:

			int socketDescriptor;
			string interfaceName;

		public:

			NICReader() {}

			NICReader(const string& interfaceName): interfaceName(interfaceName) {
				initialize(interfaceName);
			}

			~NICReader() {
				close(socketDescriptor);
			}

			void initialize(const string& interfaceName_) {
				interfaceName = interfaceName_;
				socketDescriptor = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
				if (socketDescriptor == -1) {
					notif::fatal("NIC access: error opening raw socket; possible permissions error");
				}
				notif::success("NIC access: successfully opened raw socket.");
				enablePromiscuousMode();
			}

			void enablePromiscuousMode() {
				struct ifreq ifr;
				memset(&ifr, 0, sizeof(ifr));
				strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

				if (ioctl(socketDescriptor, SIOCGIFFLAGS, &ifr) == -1) {
					notif::warning("NIC access: error getting interface flags");
					return;
				}

				ifr.ifr_flags |= IFF_PROMISC;

				if (ioctl(socketDescriptor, SIOCSIFFLAGS, &ifr) == -1) {
					notif::warning("Error enabling promiscuous mode");
				}

				notif::success("NIC access: successfully enabled promiscuous mode.");
			}

			// Receive a packet and return it
			RawPacket receiveRawPacket() {
				RawPacket pkt;
				pkt.size = recv(socketDescriptor, pkt.data, sizeof(pkt.data), 0);

				if (pkt.size == -1) {
					notif::fatal("NIC access: error receiving packet");
				}

				return pkt;
			}

			data::Bytes receiveData() {
				RawPacket pkt = receiveRawPacket();
				data::Bytes result(pkt.size);
				for (int i = 0; i < pkt.size; i++) {
					result.set(i, pkt.data[i]);
				}
				return result;
			}

			int getInterfaceIndex() {
				struct ifreq ifr;
				memset(&ifr, 0, sizeof(ifr));
				strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);

				if (ioctl(socketDescriptor, SIOCGIFINDEX, &ifr) == -1) {
					notif::fatal("NIC access: error getting interface index");
					return -1;
				}

				return ifr.ifr_ifindex;
			}

	};

	using NICR = NICReader;	

};
