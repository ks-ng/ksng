
#include "nicr.h"

namespace pktd {

	class Layer {

		public:

			virtual data::Bytes dissect(data::Bytes raw) = 0; // return the leftovers
			virtual data::Bytes assemble() = 0; // return the assembled layer

	};

	namespace layers {

		data::Bytes _ethaddrToBytes(string addr) {
			ether_addr* ethaddr = ether_aton(addr.c_str());
			unsigned char buf[6];
			for (int i = 0; i < 6; i++) { buf[i] = ethaddr->ether_addr_octet[i]; }
			data::Bytes result(6);
			for (int i = 0; i < 6; i++) { result.set(i, buf[i]); }
			return result;
		}

		data::Bytes ethaddrToBytes(string addr) {
			data::Bytes result(6);
			for (int i = 0; i < 6; i++) { result.set(i, stoi(addr.substr(3 * i, 2), 0, 16)); }
			return result;
		}

		string _bytesToEthaddr(data::Bytes raw) {
			unsigned char buf[6];
			for (int i = 0; i < 6; i++) { buf[i] = raw.get(i); }
			ether_addr* ethaddr;
			for (int i = 0; i < 6; i++) { ethaddr->ether_addr_octet[i] = buf[i]; }
			return (string)(ether_ntoa(ethaddr));
		}

		string bytesToEthaddr(data::Bytes raw, string delim=(string)(":")) {
			stringstream ss;
			for (int i = 0; i < 6; i++) {
				ss << HEX_ALPHABET[raw.get(i)];
				if (i != 5) { ss << delim; }
			}
			return ss.str();
		}

		class Ethernet: public Layer {

			public:

				string src;
				string dst;
				short etht;

				Ethernet(string src, string dst, short etht): src(src), dst(dst), etht(etht) {}

				data::Bytes dissect(data::Bytes rawData) override {
					dst = bytesToEthaddr(rawData.subbytes(0, 6));
					src = bytesToEthaddr(rawData.subbytes(6, 12));
					etht = (rawData.get(12) * 256) + rawData.get(13);
					return rawData.subbytes(15, rawData.getLength() - 1);
				}

				data::Bytes assemble() override {
					data::Bytes srcbytes = ethaddrToBytes(src);
					data::Bytes dstbytes = ethaddrToBytes(dst);
					data::Bytes ethtbytes = data::Bytes(2);
					ethtbytes.set(0, etht >> 8);
					ethtbytes.set(1, etht % 256);
					data::Bytes result(14);
					dstbytes.copyTo(result, 0);
					srcbytes.copyTo(result, 6);
					ethtbytes.copyTo(result, 12);
					return result;
				}

				string repr() {
					stringstream ss;
					ss << "[Ethernet: " << src << " -> " << dst << " (etht " << (int)(etht) << ")]";
					return ss.str();
				}

		};

	};

};
