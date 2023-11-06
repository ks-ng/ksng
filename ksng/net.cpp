#include "utils.cpp"

// Storage of raw data and fields

struct RawPacket {
	ssize_t size;
	unsigned char data[1500];
};

// Fields and Layers allow for good representation of packets
template <typename T, const int bitlength>
class NumeralField {

	public:

		T value: bitlength;

		NumeralField() {}

		NumeralField(T _value) {
			value = _value;
		}

};

template <const int length>
class BytestringField {

	public:

		Bytestring value = Bytestring(length);

		BytestringField() {}

		BytestringField(Bytestring _value) {
			if ((_value.length != length) && (length != 0)) {
				throw invalid_argument("Bad bytestring length");
			}
			_value.copyTo(value);
		}

};

class Layer {

	public:

		virtual void dissect(Bytestring data, int offset) = 0;
		virtual Bytestring assemble() = 0;

};

class Ethernet: Layer {

	public:

		BytestringField<6> src;
		BytestringField<6> dst;
		NumeralField<short, 16> etht;
		Bytestring _etht = Bytestring(2);

		Ethernet() {}

		void dissect(Bytestring _data, int offset) {
			Bytestring data = Bytestring(_data.length);
			_data.copyTo(data);

			data.substring(0 + offset, 5 + offset).copyTo(src.value);
			data.substring(6 + offset, 11 + offset).copyTo(dst.value);

			short big = data.data[12 + offset];
			short little = data.data[13 + offset];
			etht.value = (256 * big) + little;

			data.substring(12 + offset, 13 + offset).copyTo(_etht);
		}

		Bytestring assemble() {
			Bytestring result = Bytestring(14);

			src.value.copyTo(result, 0);
			dst.value.copyTo(result, 6);
			_etht.copyTo(result, 12);

			return result;
		}

};

class IPv4: Layer {

	public:

		NumeralField<char, 4> v;
		NumeralField<char, 4> ihl;
		NumeralField<char, 6> dscp;
		NumeralField<char, 2> ecn;
		NumeralField<short, 16> tlen;
		NumeralField<short, 16> id;
		NumeralField<char, 3> flags;
		NumeralField<short, 13> fragoffset;
		NumeralField<char, 8> ttl;
		NumeralField<char, 8> proto;
		NumeralField<short, 16> chk;
		BytestringField<4> src;
		BytestringField<4> dst;

		IPv4() {}

		void dissect(Bytestring _data, int offset) {
			Bytestring data = Bytestring(_data.length);
			_data.copyTo(data);

			cout << "got into dissect function" << endl;
			short bigNybble = 0;
			short smallNybble = 0;

			char b1 = data.data[0 + offset];
			v.value = (b1 & 0xF0) >> 4;
			ihl.value = (b1 & 0x0F);

			char b2 = data.data[1 + offset];
			dscp.value = b2 >> 2;
			ecn.value = b2 & 0x02;

			bigNybble = data.data[2 + offset];
			smallNybble = data.data[3 + offset];
			tlen.value = (bigNybble * 256) + smallNybble;

			bigNybble = data.data[4 + offset];
			smallNybble = data.data[5 + offset];
			id.value = (bigNybble * 256) + smallNybble;

			flags.value = data.data[6 + offset] >> 5;
			
			bigNybble = data.data[6 + offset] & 0x1f;
			smallNybble = data.data[7 + offset];
			fragoffset.value = (bigNybble * 256) + smallNybble;

			ttl.value = data.data[8 + offset];
			proto.value = data.data[9 + offset];
			
			bigNybble = data.data[10 + offset];
			smallNybble = data.data[11 + offset];
			chk.value = (bigNybble * 256) + smallNybble;

			data.substring(12 + offset, 15 + offset).copyTo(src.value);
			data.substring(16 + offset, 19 + offset).copyTo(dst.value);
		}

		Bytestring assemble() {
			return src.value;
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

	private:

		int socketDescriptor;
		std::string interfaceName;

};