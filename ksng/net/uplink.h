#include "pktd.h"

namespace uplink {

	class Uplink {

		protected:

			int sock;

		public:

			Uplink() {}

			virtual int transmit(pktd::Packet pkt, int n, bool lm) = 0;

	};

	class IPv4Uplink: public Uplink {

		public:

			IPv4Uplink() {
				sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
				if (sock < 0) {
					notif::fatal("uplink: could not establish IPv4 uplink");
				}

				int one = 1;
				const int* val = &one;
				if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
					notif::fatal("uplink: could not set socket header-inclusion option");
				}

				notif::success("uplink: successfully established uplink");
			}

			~IPv4Uplink() {
				close(sock);
			}

			int transmit(pktd::Packet pkt, int n = 1, bool lm = false) override {
				pkt.ipv4.runChecksum();
				data::Bytestream result;
				result << pkt.ipv4.assemble();
				if (pkt.ipv4.proto == 1) {
					result << pkt.icmpv4.assemble();
				} else if (pkt.ipv4.proto == 6) {
					result << pkt.tcp.assemble();
				} else if (pkt.ipv4.proto == 17) {
					result << pkt.udp.assemble();
				}
				result << pkt.payload;
				
				data::Bytes rawData = result.bytes();

				unsigned char packet[rawData.getLength()];
				for (int i = 0; i < rawData.getLength(); i++) { packet[i] = rawData.get(i); }

				struct sockaddr_in dest_addr;
				memset(&dest_addr, 0, sizeof(dest_addr));
				dest_addr.sin_family = AF_INET;
				dest_addr.sin_addr.s_addr = inet_addr(pkt.ipv4.dst.c_str());

				if (!lm) {

					int r;

					for (int i = 0; i < n; i++) {

						r = sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
						if (r < 0) {
							close(sock);
							notif::fatal("uplink: transmission failure");
						}

						if (rawData.getLength() != r) {
							stringstream ss;
							ss << "uplink: incorrect number of bytes sent; tried to send " << rawData.getLength() << " but send " << r;
							notif::error(ss.str());
						}

					}

					return r;

				} else {
					for (int i = 0; i < n; i++) { sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)); }
					return 0;
				}
			}

	};

};