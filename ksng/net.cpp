#include "utils.cpp"

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <bits/stdc++.h>

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

// Storage of raw data and fields

struct RawPacket {
	ssize_t size;
	unsigned char data[1500];
};

// Layers allow for good representation of packets

class Layer {

	public:

		virtual inline void dissect(Bytestring data, int offset=0) = 0;
		virtual Bytestring assemble() = 0;

};

class Ethernet: Layer {

	public:

		Bytestring src = Bytestring(6);
		Bytestring dst = Bytestring(6);
		unsigned short etht;
		Bytestring _etht = Bytestring(2);

		Ethernet() {}

		inline void dissect(Bytestring data, int offset=0) {
			data.substring(0 + offset, 5 + offset).copyTo(src);
			data.substring(6 + offset, 11 + offset).copyTo(dst);

			unsigned short big = data[12 + offset];
			unsigned short little = data[13 + offset];
			etht = (unsigned short)((256 * big) + little);

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

class ARP: Layer {

	public:

		unsigned short htype;
		unsigned short ptype;
		unsigned char hlen;
		unsigned char plen;
		unsigned short op;
		Bytestring sha = Bytestring(6);
		Bytestring spa = Bytestring(6);
		Bytestring tha = Bytestring(4);
		Bytestring tpa = Bytestring(4);

		inline void dissect(Bytestring data, int offset) {
			htype = (unsigned short)(data[offset] * 256) + data[1 + offset];
			ptype = (unsigned short)(data[2 + offset] * 256) + data[3 + offset];
			hlen = data[4 + offset];
			plen = data[5 + offset];
			op = (unsigned short)(data[6 + offset] * 256) + data[7 + offset];

			sha.data[0] = data[8 + offset];
			sha.data[1] = data[9 + offset];
			sha.data[2] = data[10 + offset];
			sha.data[3] = data[11 + offset];
			sha.data[4] = data[12 + offset];
			sha.data[5] + data[13 + offset];

			spa.data[0] = data[14 + offset];
			spa.data[1] = data[15 + offset];
			spa.data[2] = data[16 + offset];
			spa.data[3] = data[17 + offset];
			spa.data[4] = data[18 + offset];
			spa.data[5] + data[19 + offset];

			tha.data[0] = data[20 + offset];
			tha.data[1] = data[21 + offset];
			tha.data[2] = data[22 + offset];
			tha.data[3] = data[23 + offset];

			tpa.data[0] = data[24 + offset];
			tpa.data[1] = data[25 + offset];
			tpa.data[2] = data[26 + offset];
			tpa.data[3] = data[27 + offset];
		}

		Bytestring assemble() {
			return Bytestring(1);
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

		inline void dissect(Bytestring data, int offset=0) {
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

class IPv6: Layer {

	public:

		unsigned char v: 4;
		unsigned char tc;
		unsigned int fl: 20;
		unsigned short length;
		unsigned char next;
		unsigned char ttl;
		Bytestring src = Bytestring(16);
		Bytestring dst = Bytestring(16);

		inline void dissect(Bytestring data, int offset=0) {
			v = data[0 + offset] >> 4;
			tc = (data[0 + offset] & 0x0F) + (data[1 + offset] >> 4);
			fl = ((int)(data[1 + offset] & 0x0F) * 65536)
				 + ((int)(data[2 + offset]) * 256)
				 + (int)(data[3 + offset]);
			length = ((short)(data[4 + offset]) * 256) + (short)(data[5 + offset]);
			next = data[6 + offset];
			ttl = data[7 + offset];

			for (int i = 0; i < 16; i++) { 
				src.data[i] = data[i + 8 + offset];
				dst.data[i] = data[i + 24 + offset];
			}
		}

		Bytestring assemble() {
			return src;
		}

};

class ICMP: Layer {

	public:

		unsigned char type;
		unsigned char code;
		unsigned short chk;
		unsigned int data;

		inline void dissect(Bytestring data_, int offset=0) {
			type = data_[0 + offset];
			code = data_[1 + offset];
			chk = (unsigned short)(data_[2 + offset] * 256) + (unsigned short)(data_[3 + offset]);
			data = (unsigned int)(data_[4 + offset] * 16777216) 
				+ (unsigned int)(data_[5 + offset] * 65536) 
				+ (unsigned int)(data_[6 + offset] * 256) 
				+ (unsigned int)(data_[7 + offset]);
		}

		Bytestring assemble() {
			return nullString(2);
		}

};

class TCP: Layer {

	public:

		unsigned short srcp;
		unsigned short dstp;
		unsigned int seq;
		unsigned char dataOffset: 4;
		unsigned char flags;
		unsigned short window;
		unsigned short chk;
		unsigned short urg;

		inline void dissect(Bytestring data, int offset=0) {
			srcp = (unsigned short)(data[offset] * 256) + data[1 + offset];
			dstp = (unsigned short)(data[2 + offset] * 256) + data[3 + offset];
			seq = (unsigned int)(data[4 + offset] * 16777216) 
				+ (unsigned int)(data[5 + offset] * 65536) 
				+ (unsigned int)(data[6 + offset] * 256) 
				+ data[7 + offset];
			dataOffset = data[8 + offset] >> 4;
			flags = data[9 + offset];
			window = (unsigned short)(data[10 + offset] * 256) + data[11 + offset];
			chk = (unsigned short)(data[12 + offset] * 256) + data[13 + offset];
			urg = (unsigned short)(data[14 + offset] * 256) + data[15 + offset];
		}

		Bytestring assemble() {
			return Bytestring(0);
		}

};

class UDP: Layer {

	public:

		unsigned short srcp;
		unsigned short dstp;
		unsigned short length;
		unsigned short chk;

		inline void dissect(Bytestring data, int offset=0) {
			srcp = (unsigned short)(data[offset] * 256) + data[1 + offset];
			dstp = (unsigned short)(data[2 + offset] * 256) + data[3 + offset];
			length = (unsigned short)(data[4 + offset] * 256) + data[5 + offset];
			chk = (unsigned short)(data[6 + offset] * 256) + data[7 + offset];
		}

		Bytestring assemble() {
			return Bytestring(0);
		}

};

class Packet {

	public:

		int layerCount;
		Layer** layers;

		void initialize(const int size) {
			layerCount = size;
			layers = new Layer*[size];
		}

		Packet() {}

		Packet(const int size) {
			initialize(size);
		}

};

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

class ProtocolInterface {

	public:

		virtual ~ProtocolInterface() {}

		virtual void send(const Bytestring& data) = 0;
		virtual Bytestring receive() = 0;
};

class TCPInterface : public ProtocolInterface {

	public:

		TCPInterface(const std::string& ipAddress, uint16_t port) {
			socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
			if (socketDescriptor == -1) {
				std::cerr << "TCP socket creation failed" << std::endl;
				// Handle the error
			}

			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(port);
			inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);

			if (connect(socketDescriptor, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
				std::cerr << "TCP connection failed" << std::endl;
				// Handle the error
			}
		}

		~TCPInterface() override {
			close(socketDescriptor);
		}

		void send(const Bytestring& data) override {
			if (::send(socketDescriptor, data.data, data.length, 0) == -1) {
				std::cerr << "Error sending data over TCP" << std::endl;
				// Handle the error
			}
		}

		Bytestring receive() override {
			Bytestring receivedData;
			receivedData.length = recv(socketDescriptor, receivedData.data, sizeof(receivedData.data), 0);
			return receivedData;
		}

	private:

		int socketDescriptor;
		sockaddr_in serverAddr;

};

class UDPInterface : public ProtocolInterface {

	public:

		UDPInterface(const std::string& ipAddress, uint16_t port) {
			socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
			if (socketDescriptor == -1) {
				std::cerr << "UDP socket creation failed" << std::endl;
				// Handle the error
			}

			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(port);
			inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);
		}

		~UDPInterface() override {
			close(socketDescriptor);
		}

		void send(const Bytestring& data) override {
			if (sendto(socketDescriptor, data.data, data.length, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
				std::cerr << "Error sending data over UDP" << std::endl;
				// Handle the error
			}
		}

		Bytestring receive() override {
			Bytestring receivedData;
			sockaddr_in clientAddr;
			socklen_t clientLen = sizeof(clientAddr);

			receivedData.length = recvfrom(socketDescriptor, receivedData.data, sizeof(receivedData.data), 0,
										(struct sockaddr*)&clientAddr, &clientLen);
			return receivedData;
		}

	private:

		int socketDescriptor;
		sockaddr_in serverAddr;

};