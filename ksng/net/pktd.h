
#include "nicr.h"

namespace pktd {

	class Layer {

		public:

			virtual data::Bytes dissect(data::Bytes raw) = 0; // return the leftovers
			virtual data::Bytes assemble() = 0; // return the assembled layer

	};

	namespace layers {

		data::Bytes ethaddrToBytes(string addr) {
			ether_addr* ethaddr = ether_aton(addr.c_str());
			unsigned char buf[6];
			for (int i = 0; i < 6; i++) { buf[i] = ethaddr->ether_addr_octet[i]; }
			data::Bytes result(6);
			for (int i = 0; i < 6; i++) { result.set(i, buf[i]); }
			return result;
		}

		string bytesToEthaddr(data::Bytes raw) {
			unsigned char buf[6];
			for (int i = 0; i < 6; i++) { buf[i] = raw.get(i); }
			ether_addr* ethaddr;
			for (int i = 0; i < 6; i++) { ethaddr->ether_addr_octet[i] = buf[i]; }
			return (string)(ether_ntoa(ethaddr));
		}

		class Ethernet: public Layer {

			public:

				string src;
				string dst;
				short etht;

				Ethernet(string src, string dst, short etht): src(src), dst(dst), etht(etht) {}

				data::Bytes dissect(data::Bytes rawData) override {
					cout << "dissecting ethernet ...";
					dst = bytesToEthaddr(rawData.subbytes(0, 6));
					cout << "dst done ...";
					src = bytesToEthaddr(rawData.subbytes(6, 12));
					cout << "src done ...";
					etht = (rawData.get(12) * 256) + rawData.get(13);
					cout << "etht done." << endl;
					return rawData.subbytes(15, rawData.getLength() - 1);
				}

				data::Bytes assemble() override {
					cout << "assembling ethernet ...";
					data::Bytes srcbytes = ethaddrToBytes(src);
					data::Bytes dstbytes = ethaddrToBytes(dst);
					data::Bytes ethtbytes = data::Bytes(2);
					cout << "built bytes ...";
					ethtbytes.set(0, etht >> 8);
					ethtbytes.set(1, etht % 256);
					cout << "built etht ...";
					data::Bytes result(14);
					dstbytes.copyTo(result, 0);
					cout << "copied dst ...";
					srcbytes.copyTo(result, 6);
					cout << "copied src ...";
					ethtbytes.copyTo(result, 12);
					cout << "done." << endl;
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