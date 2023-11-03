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

		Bytestring value = Bytestring(defaultLength);

		BytestringField() {}

		BytestringField(Bytestring _value) {
			if (_value.length != length) {
				throw invalid_argument("Bad bytestring length");
			}
			_value.copyTo(value);
		}

};

class Layer {

	public:

		virtual Bytestring dissect(Bytestring data) = 0;
		virtual Bytestring assemble() = 0;

};

class Ethernet: Layer {

	public:

		BytestringField<6> src;
		BytestringField<6> dst;
		NumeralField<short, 16> etht;
		Bytestring _etht = Bytestring(2);

		Bytestring dissect(Bytestring data) {
			data.substring(0, 5).copyTo(src.value);
			data.substring(6, 11).copyTo(dst.value);

			short big = data.data[12];
			short little = data.data[13];
			etht.value = (256 * big) + little;

			data.substring(12, 13).copyTo(_etht);

			return data.substring(14, data.length - 1);
		}

		Bytestring assemble() {
			Bytestring result = Bytestring(14);

			src.value.copyTo(result, 0);
			dst.value.copyTo(result, 6);
			_etht.copyTo(result, 12);

			return result;
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
			Bytestring result = Bytestring(pkt.data, 1500);
			return result;
		}

	private:

		int socketDescriptor;
		std::string interfaceName;

};