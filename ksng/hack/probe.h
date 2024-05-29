#include "../net/conn.h"

namespace probe {

	void spy(string interface, int count, bool lm) {
		pktd::PacketDissector dsctr(interface);
		cout << "Initializing spying.\n  Interface name: " << interface << endl;
		cout << "  Desired number of packets: " << count << " packets";
		cout << endl;
		cout << "  Interface connection: " << colors::colorize("ARMED", colors::OKGREEN) << endl;
		data::Bytes rawData;
		bool go = false;
		if (lm) { go = true; } else { go = notif::confirm(); }
		if (go) {
			cout << "Spying on " << interface << " ..." << endl;

			int i = count;
			while (i --> 0) {
				cout << dsctr.receivePacket().repr() << endl;
			}
		}
		cout << "Spying operation complete." << endl;
	}

};