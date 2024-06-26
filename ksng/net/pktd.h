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
			virtual string reportString() = 0;
			void report() { cout << reportString() << endl; }

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

		data::Bytes ipv6ToBytes(string addr) {
			return data::Bytes({0});
		}

		string bytesToIPv6(data::Bytes raw) {
			return "";
		}

		unsigned short internetChecksum(data::Bytes rawData) {
			if (rawData.getLength() % 2 == 1) {
				data::Bytes nd(rawData.getLength() + 1);
				rawData.copyTo(nd);
				rawData = nd;
				rawData.set(rawData.getLength() - 1, 0); // just making sure
			}

			sda::SDA<unsigned short> shorts((rawData.getLength() / 2));
			for (int i = 0; i < (rawData.getLength()) / 2; i++) {
				shorts.set(i, rawData.getShort(2 * i));
			}
			unsigned int s = 0;
			for (int i = 0; i < shorts.getLength(); i++) {
				s += static_cast<unsigned int>(shorts.get(i));
			}

			unsigned int r = s % 65536;
			s = (s & 0xFFFF) + r;

			return ~(static_cast<unsigned short>(s));
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

				string reportString() override {
					stringstream ss;
					ss << " === Ethernet === " << endl;
					ss << "Source MAC address: " << src << endl;
					ss << "Destination MAC address: " << dst << endl;
					ss << "Ethertype: " << etht << endl;
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

				string reportString() override {
					stringstream ss;
					ss << " === Address Resolution Protocol === " << endl;
					ss << "Hardware type: " << htype << endl;
					ss << "Protocol type: " << ptype << endl;
					ss << "Hardware address length: " << hlen << endl;
					ss << "Protocol address length: " << plen << endl;
					ss << "Operation: " << op << endl;
					ss << "Sender hardware address: " << sha << endl;
					ss << "Sender protocol address: " << spa << endl;
					ss << "Target hardware address: " << tha << endl;
					ss << "Target protocol address: " << tpa << endl;
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

				IPv4() {
					ver = 4;
					ihl = 5;
					dscp = 0;
					ecn = 0;
					ipid = 0;
					flags = 0;
					fragoff = 0;
					ttl = 255;
					proto = 0;
				}

				IPv4(string s, string d, int pro) {
					ver = 4;
					ihl = 5;
					dscp = 0;
					ecn = 0;
					ipid = 0;
					flags = 0;
					fragoff = 0;
					ttl = 255;
					proto = pro;
					src = s;
					dst = d;
				}

				data::Bytes dissect(data::Bytes rawData) override {
					cout << "ipv4 d" << rawData.hex() << endl;
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
					report();
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
					chk = 0;
					chk = internetChecksum(result);
					result.loadShort(chk, 10);
					return result;
				}

				void runChecksum() {
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
					chk = internetChecksum(result);
				}

				string repr() override {
					stringstream ss;
					ss << "[IPv" << (unsigned int)(ver) << ": " << src << " -> " << dst << " (proto " << (unsigned int)(proto) << ")]";
					return ss.str();
				}

				string reportString() override {
					stringstream ss;
					ss << " === Internet Protocol === " << endl;
					ss << "IP version: " << (unsigned int)(ver) << endl;
					ss << "Internet header length: " << (unsigned int)(ihl) << endl;
					ss << "Differentiated services codepoint: " << (unsigned int)(dscp) << endl;
					ss << "Explicit congestion notification: " << (unsigned int)(ecn) << endl;
					ss << "IP header length: " << length << endl;
					ss << "IP identification: " << ipid << endl;
					ss << "Flags: " << (unsigned int)(flags) << endl;
					ss << "Fragmentation offset: " << fragoff << endl;
					ss << "Time to live: " << (unsigned int)(ttl) << endl;
					ss << "Protocol: " << (unsigned int)(proto) << endl;
					ss << "Checksum: " << chk << endl;
					ss << "Source address: " << src << endl;
					ss << "Destination address: " << dst << endl;
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

				string reportString() override {
					stringstream ss;
					ss << " === Internet Control Message Protocol === " << endl;
					ss << "Type: " << type << endl;
					ss << "Code: " << code << endl;
					ss << "Checksum: " << chk << endl;
					ss << "Header data: " << roh.hex() << endl;
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

				string reportString() override {
					stringstream ss;
					ss << " === Transmission Control Protocol === " << endl;
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
					return rawData.subbytes(8, rawData.getLength());
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

				string reportString() override {
					stringstream ss;
					ss << " === User Datagram Protocol === " << endl;
					cout << "Source port: " << srcp << endl;
					cout << "Destination port: " << dstp << endl;
					cout << "Length: " << length << endl; 
					cout << "Checksum: " << chk << endl;
					return ss.str();
				}

		};

		class IPv6: public Layer {
		
			public:

				unsigned char ver;
				unsigned char traffic;
				unsigned int flow;
				unsigned short paylength;
				unsigned char proto;
				unsigned char ttl;
				string src;  
				string dst;

				data::Bytes dissect(data::Bytes rawData) override {
					dissected = true;
					ver = rawData[0] >> 4;
					traffic = (rawData[0] & 0x0F) + (rawData[1] >> 4);
					flow = (rawData[1] & 0x0F) + rawData.getShort(2);
					paylength = rawData.getShort(4);
					proto = rawData[6];
					ttl = rawData[7];
					src = bytesToIPv6(rawData.subbytes(8, 24));
					dst = bytesToIPv6(rawData.subbytes(24, 40));
					return rawData.subbytes(40, rawData.getLength());
				}

				data::Bytes assemble() override {
					data::Bytestream finalResult;
					data::Bytes result(40);
					result.set(0, (ver << 4) + (traffic >> 4));
					result.set(1, ((traffic & 0xF0) << 4) + (flow >> 16));
					result.loadShort(2, flow & 0xFFFF);
					result.loadShort(4, paylength);
					result.set(6, proto);
					result.set(7, ttl);
					finalResult << result;
					finalResult << ipv6ToBytes(src); 
					finalResult << ipv6ToBytes(dst); 
					return finalResult.bytes();
				}

				string repr() override {
					stringstream ss; 
					ss << "[IPv" << (unsigned int)(ver) << ": " << src << " -> " << dst << " (proto " << (unsigned int)(proto) << ")]";
					return ss.str();
				}

				string reportString() override {
					stringstream ss;
					ss << " === Internet Protocol version 6 === " << endl;
					ss << "IP version: " << (unsigned int)(ver) << endl;
					ss << "Traffic class: " << (unsigned int)(traffic);
					ss << "Flow label: " << flow << endl; 
					ss << "Payload length: " << paylength << endl;
					ss << "Protocol (\"next header\"): " << proto << endl; 
					ss << "Time to live (\"hop limit\"): " << ttl << endl;
					ss << "Source address: " << src << endl; 
					ss << "Destination address: " << dst << endl;
					return ss.str();
				}

		};

	};

	class Packet {

		public:

			layers::Ethernet eth;
			layers::ARP arp;
			layers::IPv4 ipv4;
			layers::IPv6 ipv6;
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

			void fixUDP(bool runChecksum=false) {
				udp.length = (unsigned short)(payload.getLength()) + 8;
				udp.chk = 0;
				if (runChecksum) {
					data::Bytestream ipv4ph;
					ipv4ph << layers::ipv4ToBytes(ipv4.src);
					ipv4ph << layers::ipv4ToBytes(ipv4.dst);
					ipv4ph << data::Bytes({0x00, 0x11});
					data::Bytes udplen(2);
					udplen.loadShort((unsigned short)(udp.length));
					ipv4ph << udplen;
					ipv4ph << udp.assemble();
					ipv4ph << payload;
					cout << ipv4ph.bytes().hex() << endl;
					udp.chk = layers::internetChecksum(ipv4ph.bytes());
				}
			}
			
			void fixIPv4Length() {
				if (ipv4.proto == 1) {
					ipv4.length = 28;
				} else if (ipv4.proto == 6) {
					ipv4.length = 40;
				} else if (ipv4.proto == 17) {
					ipv4.length = 24;
				}
				ipv4.length += 4 + payload.getLength(); // don't ask why I need to add the 4.
			}

			void fixIPv6Length() {
				if (ipv6.proto == 1) {
					ipv6.paylength = 8;
				} else if (ipv6.proto == 6) {
					ipv6.paylength = 20;
				} else if (ipv6.proto == 17) {
					ipv6.paylength = 4;
				}
				ipv6.paylength += payload.getLength();
			}

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
				if (eth.etht == 34525) {
					ss << " / " << ipv6.repr();
					if (ipv6.proto == 1) {
						ss << " / " << icmpv4.repr();
					} else if (ipv6.proto == 6) {
						ss << " / " << tcp.repr();
					} else if (ipv6.proto == 17) {
						ss << " / " << udp.repr();
					}
				}
				ss << " / " << payload.getLength() << "-byte payload";
				return ss.str();
			}

			data::Bytes assemble() {
				data::Bytestream result;
				result << eth.assemble();
				if (eth.etht == 2054) {
					result << arp.assemble();
				}
				if (eth.etht == 2048) {
					result << ipv4.assemble();
					if (ipv4.proto == 1) {
						result << icmpv4.assemble();
					} else if (ipv4.proto == 6) {
						result << tcp.assemble();
					} else if (ipv4.proto == 17) {
						result << udp.assemble();
					}
				}
				if (eth.etht == 34525) {
					result << ipv6.assemble();
					if (ipv6.proto == 1) {
						result << icmpv4.assemble();
					} else if (ipv6.proto == 6) {
						result << tcp.assemble();
					} else if (ipv6.proto == 17) {
						result << udp.assemble();
					}
				}
				result << payload;
				return result.bytes();
			}

			void report() {
				eth.report();
				if (eth.etht == 2054) {
					arp.report();
				}
				if (eth.etht == 2048) {
					ipv4.report();
					if (ipv4.proto == 1) {
						icmpv4.report();
					} else if (ipv4.proto == 6) {
						tcp.report();
					} else if (ipv4.proto == 17) {
						udp.report();
					}
				}
				if (eth.etht == 34525) {
					ipv6.report();
					if (ipv6.proto == 1) {
						icmpv4.report();
					} else if (ipv6.proto == 6) {
						tcp.report();
					} else if (ipv6.proto == 17) {
						udp.report();
					}
				}
			}

	};

	class PacketDissector {

		public:

			nicr::NICR nr;

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
					} else {
						stringstream ss; 
						ss << "packet dissector: odd protocol: " << (int)(result.ipv4.proto);
						notif::warning(ss.str());
						return result;
					}
				}
				if (result.eth.etht == 34525) {
					rawData = result.ipv6.dissect(rawData);
					if (result.ipv6.proto == 1) {
						result.payload = result.icmpv4.dissect(rawData);
						return result;
					} else if (result.ipv6.proto == 6) {
						result.payload = result.tcp.dissect(rawData);
						return result;
					} else if (result.ipv6.proto == 17) {
						result.payload = result.udp.dissect(rawData);
						return result;
					}
				}
				result.payload = rawData;
				return result;
			}

			Packet receivePacket() {
				data::Bytes d = nr.receiveData();
				cout << "pktd " << d.hex() << endl;
				return dissectPacket(d);
			}

			void sendPacket(Packet pkt) {
				nr.sendData(pkt.assemble());
			}

	};

};
