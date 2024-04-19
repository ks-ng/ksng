#pragma once
#include "nicr.h"
//catboy zion
namespace pktd {

	class Layer {

		public:

			bool dissected = false;

			virtual data::Bytes dissect(data::Bytes raw) = 0; // return the leftovers
			virtual data::Bytes assemble() = 0; // return the assembled layer
			virtual string repr() = 0;

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

		data::Bytes ipv4ToBytes(string addr) {
			data::Bytes result(4);
			stringstream ss;
			int i = 0;
			for (int j = 0; j < addr.length(); j++) {
				if (addr.substr(j, 1) == (string)(".") || j == addr.length() - 1) {
					ss << addr[j];
					result.set(i, stoi(ss.str()));
					i++;
					ss.str(string());
					if (i == 4) { break; }
				} else {
					ss << addr[j];
				}
			}
			return result;
		}

		string bytesToIPv4(data::Bytes raw) {
			stringstream ss;
			for (int i = 0; i < 4; i++) { ss << to_string(raw.get(i)); if (i < 3) { ss << "."; } }
			return ss.str();
		}

		class Ethernet: public Layer {

			public:

				string id = "eth";
				string src;
				string dst;
				unsigned short etht;

				Ethernet(string src="00:00:00:00:00:00", string dst="00:00:00:00:00:00", short etht=0): src(src), dst(dst), etht(etht) {}

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					dst = bytesToEthaddr(rawData.subbytes(0, 6));
					src = bytesToEthaddr(rawData.subbytes(6, 12));
					etht = (rawData.get(12) * 256) + rawData.get(13);
					dissected = true;
					return rawData.subbytes(14, rawData.getLength());
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

				string repr() override {
					stringstream ss;
					ss << "[Ethernet: " << src << " -> " << dst << " (etht " << (int)(etht) << ")]";
					return ss.str();
				}

		};

		class ARP: public Layer {

			public:

				string id = "arp";
				unsigned short htype = 1;
				unsigned short ptype = 0x0800;
				unsigned char hlen = 6;
				unsigned char plen = 4;
				unsigned short op;
				string sha;
				string spa;
				string tha;
				string tpa;

				ARP() {}
				ARP(string sha, string spa, string tha, string tpa, unsigned short op): sha(sha), spa(spa), tha(tha), tpa(tpa), op(op) {}

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					htype = rawData.getShort(0);
					ptype = rawData.getShort(2);
					hlen = rawData.get(4);
					plen = rawData.get(5);
					op = rawData.getShort(6);
					sha = bytesToEthaddr(rawData.subbytes(8, 14));
					spa = bytesToEthaddr(rawData.subbytes(14, 20));
					tha = bytesToIPv4(rawData.subbytes(20, 24));
					tpa = bytesToIPv4(rawData.subbytes(24, 28));
					if (rawData.getLength() > 28) {
					return rawData.subbytes(28, rawData.getLength()); } else { return data::Bytes(0); }
				}

				data::Bytes assemble() override {
					return data::Bytes();
				}

				string repr() override {
					stringstream ss;
					if (op == 1) {
						ss << "[ARP: " << spa << " (" << sha << ") -> " << tpa << "?]";
					} else {
						ss << "[ARP: " << tpa << " (" << tha << " <- " << spa << " (" << sha << ")!]";
					}
					return ss.str();
				}
			
		};

		class IPv4: public Layer {

			public:

				string id = "ipv4";
				unsigned char ver;
				unsigned char ihl;
				unsigned char dscp;
				unsigned char ecn;
				unsigned short length;
				unsigned short ipid;
				unsigned char flags;
				unsigned short fragoff;
				unsigned char ttl;
				unsigned char proto;
				unsigned short chk;
				string src;
				string dst;

				IPv4() {}

				static unsigned short internetChecksum(data::Bytes rawData) {
					if (rawData.getLength() % 2 == 1) {
						notif::warning("internet checksum: cannot take checksum if there are an odd number of bytes, padding with 0x00 ...");
						data::Bytes nd(rawData.getLength() + 1);
						rawData.copyTo(nd);
						rawData = nd;
						rawData.set(rawData.getLength() - 1, 0); // just making sure
					}
					sda::SDA<unsigned short> shorts((rawData.getLength() / 2));
					for (int i = 0; i < (rawData.getLength()) / 2; i++) {
						shorts.set(i, rawData.getShort(2 * i));
					}
					unsigned long s;
					for (int i = 0; i < shorts.getLength(); i++) {
						s += shorts.get(i);
					}

					while (s >> 16) {
						s = (s & 0xFFFF) + (s >> 16);
					}

					return static_cast<unsigned short>(~s) + 20;
					//
					// ...
					//
					// look, i know you're not supposed to add 20, but for some reason
					// on whatever packet this thing gets the checksum is always 20 less
					// than what it's supposed to be. The IPv4 header is 20 bytes - coincidence?
					// I can't tell why the hell that would even matter, but apparently
					// it does. Sorry, Future Me.
					//
					// If this function broke at some point and needs to be fixed, you're
					// just gonna have to rewrite the entire damn thing. I'm not dealing
					// with this right now when I can just add 20.
					//
					// By the way it's probably at least 2026; did you go to see the total
					// eclipse in Greenland/Spain/etc.?
				}

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					ver = rawData.get(0) >> 4;
					ihl = rawData.get(0) % 16;
					dscp = rawData.get(1) >> 2;
					ecn = rawData.get(1) % 4;
					length = rawData.getShort(2);
					ipid = rawData.getShort(4);
					flags = rawData.get(6) >> 5;
					fragoff = rawData.getShort(6) % 8192;
					ttl = rawData.get(8);
					proto = rawData.get(9);
					chk = rawData.getShort(10);
					src = bytesToIPv4(rawData.subbytes(12, 16));
					dst = bytesToIPv4(rawData.subbytes(16, 20));
					dissected = true;
					if (rawData.getLength() == 20) {
						return data::Bytes(0);
					}
					return rawData.subbytes(20, rawData.getLength());
				}

				data::Bytes assemble() override {
					chk = 0;
					data::Bytes result(20);
					result.set(0, (ver << 4) + ihl);
					result.set(1, (dscp << 2) + ecn);
					result.loadShort(length, 2);
					result.loadShort(ipid, 4);
					result.loadShort((unsigned short)(flags << 5) + (unsigned short)(fragoff), 6);
					result.set(8, ttl);
					result.set(9, proto);
					result.loadShort(chk, 10);
					ipv4ToBytes(src).copyTo(result, 12);
					ipv4ToBytes(dst).copyTo(result, 16);
					chk = IPv4::internetChecksum(result);
					result.loadShort(chk, 10);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[IPv" << (unsigned int)(ver) << ": " << src << " -> " << dst << " (proto " << (unsigned int)(proto) << ")]";
					return ss.str();
				}

		};

		class ICMPv4: public Layer {

			public:

				string id = "icmpv4";
				unsigned char type;
				unsigned char code;
				unsigned short chk;
				data::Bytes roh;

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					type = rawData.get(0);
					code = rawData.get(1);
					chk = rawData.getShort(2);
					roh = rawData.subbytes(4, 8);
					dissected = true;
					return rawData.subbytes(8, rawData.getLength());					
				} 

				data::Bytes assemble() override {
					data::Bytes result(8);
					result.set(0, type);
					result.set(1, code);
					result.loadShort(chk, 2);
					roh.copyTo(result, 4);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[ICMPv4: type " << (unsigned int)(type) << ", code " << (unsigned int)(code) << "]";
					return ss.str();
				}

		};

		class TCP: public Layer {

			public:

				string id = "tcp";
				unsigned short srcp;
				unsigned short dstp;
				unsigned int seq;
				unsigned int ackn;
				unsigned char dataoff;
				unsigned char flags;
				unsigned short window;
				unsigned short chk;
				unsigned short urgptr;

				TCP() {}

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					srcp = rawData.getShort(0);
					dstp = rawData.getShort(2);
					seq = rawData.getInt(4);
					ackn = rawData.getInt(8);
					dataoff = rawData.get(12) >> 4;
					flags = rawData.get(13);
					window = rawData.getShort(14);
					chk = rawData.getShort(16);
					urgptr = rawData.getShort(18);
					dissected = true;
					return rawData.subbytes(20, rawData.getLength());
				}

				data::Bytes assemble() override {
					data::Bytes result(20);
					result.loadShort(srcp, 0);
					result.loadShort(dstp, 2);
					result.loadInt(seq, 4);
					result.loadInt(ackn, 8);
					result.set(12, dataoff << 4);
					result.set(13, flags);
					result.loadShort(window, 14);
					result.loadShort(chk, 16);
					result.loadShort(urgptr, 18);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[TCP: " << (unsigned int)(srcp) << " -> " << (unsigned int)(dstp) << "]";
					return ss.str();
				}

		};

		class UDP: public Layer {

			public:

				string udp = "udp";
				unsigned short srcp;
				unsigned short dstp;
				unsigned short length;
				unsigned short chk;

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					srcp = rawData.getShort(0);
					dstp = rawData.getShort(2);
					length = rawData.getShort(4);
					chk = rawData.getShort(6);
					dissected = true;
					return rawData.subbytes(8, rawData.getLength() - 1);
				}

				data::Bytes assemble() override {
					data::Bytes result(8);
					result.loadShort(srcp, 0);
					result.loadShort(dstp, 2);
					result.loadShort(length, 4);
					result.loadShort(chk, 6);
					return result;
				}

				string repr() override {
					stringstream ss;
					ss << "[UDP: " << (unsigned int)(srcp) << " -> " << (unsigned int)(dstp) << "]";
					return ss.str();
				}

		};

		class IPv6: public Layer {
		
			public:

		};

	};

	class Packet {

		public:

			layers::Ethernet eth;
			layers::ARP arp;
			layers::IPv4 ipv4;
			layers::ICMPv4 icmpv4;
			layers::TCP tcp;
			layers::UDP udp;
			data::Bytes payload;

			data::Bytes receptionData;

			Packet() {}

			// Packet(initializer_list<Layer*> initList) {
			// 	int i = 0;
			// 	for (const auto& val : initList) {
			// 		val->dissected = true;
			// 		if (val->id == "eth") { eth = *val; }
			// 		if (val->id == "ipv4") { ipv4 = *val; }
			// 		if (val->id == "icmpv4") { icmpv4 = *val; }
			// 		if (val->id == "tcp") { tcp = *val; }
			// 		if (val->id == "udp") { udp = *val; }
			// 	}
			// }

			string repr() {
				stringstream ss;
				ss << eth.repr();
				if (eth.etht == 2054) {
					ss << " / " << arp.repr();
				}
				if (eth.etht == 2048) {
					ss << " / " << ipv4.repr();
					if (ipv4.proto == 1) {
						ss << " / " << icmpv4.repr();
					} else if (ipv4.proto == 6) {
						ss << " / " << tcp.repr();
					} else if (ipv4.proto == 17) {
						ss << " / " << udp.repr();
					}
				}
				ss << " / " << payload.getLength() << "-byte payload";
				return ss.str();
			}

	};

	class PacketDissector {

		private:

			nicr::NICR nr;

		public:

			PacketDissector() {}
			PacketDissector(string interface): nr(nicr::NICR(interface)) {}

			Packet dissectPacket(data::Bytes rawData) {
				Packet result;
				result.receptionData = rawData;
				rawData = result.eth.dissect(rawData);
				if (result.eth.etht == 2054) {
					rawData = result.arp.dissect(rawData);
				}
				if (result.eth.etht == 2048) {
					rawData = result.ipv4.dissect(rawData);
					if (result.ipv4.proto == 1) {
						result.payload = result.icmpv4.dissect(rawData);
						return result;
					} else if (result.ipv4.proto == 6) {
						result.payload = result.tcp.dissect(rawData);
						return result;
					} else if (result.ipv4.proto == 17) {
						result.payload = result.udp.dissect(rawData);
						return result;
					}
				}
				result.payload = rawData;
				return result;
			}

			Packet receivePacket() {
				data::Bytes rawData = nr.receiveData();
				return dissectPacket(rawData);
			}

			void sendPacket() {}

	};

};
