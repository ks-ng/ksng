#include "utils.cpp"

// Storage of raw data and fields

struct RawPacket {
	ssize_t size;
	unsigned char data[1500];
};

// Layers allow for good representation of packets

class Layer {

	public:

		virtual void dissect(Bytestring data, int offset=0) = 0;
		virtual Bytestring assemble() = 0;

};

class Ethernet: Layer {

	public:

		Bytestring src = Bytestring(6);
		Bytestring dst = Bytestring(6);
		unsigned short etht;
		Bytestring _etht = Bytestring(2);

		Ethernet() {}

		void dissect(Bytestring data, int offset=0) {
			data.substring(0 + offset, 5 + offset).copyTo(src);
			data.substring(6 + offset, 11 + offset).copyTo(dst);

			unsigned short big = data[12 + offset];
			unsigned short little = data[13 + offset];
			etht = (short)((256 * big) + little);

			data.substring(12 + offset, 13 + offset).copyTo(_etht);
		}

		Bytestring assemble() {
			Bytestring result = Bytestring(14);

			src.copyTo(result, 0);
			dst.copyTo(result, 6);
			_etht.copyTo(result, 12);

			return result;
		}

};

class IPv4: Layer {

	public:

		unsigned char v: 4;
		unsigned char ihl: 4;
		unsigned char dscp: 6;
		unsigned char ecn: 2;
		unsigned short tlen;
		unsigned short id;
		unsigned char flags: 3;
		unsigned short fragoffset: 13;
		unsigned char ttl;
		unsigned char proto;
		unsigned short chk;
		Bytestring src = Bytestring(4);
		Bytestring dst = Bytestring(4);

		IPv4() {}

		void dissect(Bytestring data, int offset=0) {
			v = data[0 + offset] >> 4;
			ihl = data[0 + offset] & 0x0F;

			dscp = data[1 + offset] >> 2;
			ecn = data[1 + offset] & 0x3;

			tlen = (short)(data[2 + offset] * 256) + data[3 + offset];
			id = (short)(data[4 + offset] * 256) + data[5 + offset];
			flags = data[6 + offset] >> 5;
			fragoffset = (short)((data[6 + offset] & 0x1f) * 256) + data[7 + offset];
			ttl = data[8 + offset];
			proto = data[9 + offset];
			chk = (short)(data[10 + offset] * 256) + data[11 + offset];

			//Bytestring srcSub = data.substring(12 + offset, 15 + offset);
			src.data[0] = data[12 + offset];
			src.data[1] = data[13 + offset];
			src.data[2] = data[14 + offset];
			src.data[3] = data[15 + offset];

			dst.data[0] = data[16 + offset];
			dst.data[1] = data[17 + offset];
			dst.data[2] = data[18 + offset];
			dst.data[3] = data[19 + offset];
		}

		Bytestring assemble() {
			return src;
		}
};

class TCP: Layer {

	public:

		short srcp;
		short dstp;
		int seq;
		char dataOffset: 4;
		char flags;
		short window;
		short chk;
		short urg;

		void dissect(Bytestring data, int offset=0) {
			srcp = (short)(data[offset] * 256) + data[1 + offset];
			dstp = (short)(data[2 + offset] * 256) + data[3 + offset];
			seq = (int)(data[4 + offset] * 16777216) 
				+ (int)(data[5 + offset] * 65536) 
				+ (int)(data[6 + offset] * 256) 
				+ data[7 + offset];
			dataOffset = data[8 + offset] >> 4;
			flags = data[9 + offset];
			window = (short)(data[10 + offset] * 256) + data[11 + offset];
			chk = (short)(data[12 + offset] * 256) + data[13 + offset];
			urg = (short)(data[14 + offset] * 256) + data[15 + offset];
		}

		Bytestring assemble() {
			return;
		}

}

class UDP: Layer {

	public:

		short srcp;
		short dstp;
		short length;
		short chk;

		void dissect(Bytestring data, int offset=0) {
			srcp = (short)(data[offset] * 256) + data[1 + offset];
			dstp = (short)(data[2 + offset] * 256) + data[3 + offset];
			length = (short)(data[4 + offset] * 256) + data[5 + offset];
			chk = (short)(data[6 + offset] * 256) + data[7 + offset];
		}

		Bytestring assemble() {
			return;
		}

}

// This code is absolutely terrifying but it works so
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
		RawPacket receiveRawPacket() {
			RawPacket pkt;
			pkt.size = recv(socketDescriptor, pkt.data, sizeof(pkt.data), 0);

			if (pkt.size == -1) {
				std::cerr << "Error receiving packet" << std::endl;
			}

			return pkt;
		}

		Bytestring receiveData() {
			RawPacket pkt = receiveRawPacket();
			Bytestring result = Bytestring(pkt.data, pkt.size);
			return result;
		}

	private:

		int socketDescriptor;
		std::string interfaceName;

};
