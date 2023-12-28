#include "../util/fileops.h"

using namespace std;

namespace hazmat {

	bool HAZMAT_ACCESS_ALLOWED = false;

	void hazmatWarning() {
		if (HAZMAT_ACCESS_ALLOWED) { return; }

		cout << colors::colorize("Kingslayer-NG: hazardous materials warning: hazmat was requested. Grant access? (y/n)", colors::WARNING) << endl;
		string usr;
		cin >> usr;

		if (usr == "yes" || usr == "y") {
			cout << colors::colorize("Hazardous materials access granted.", colors::WARNING) << endl;
			HAZMAT_ACCESS_ALLOWED = true;
		} else {
			notif::fatal("hazmat access denied");
		}
	}

	data::Bytes read(string name) {
		hazmatWarning();
		return fileops::readFileBytes("ksng/hazmat/hazmat/" + name + ".hazmat");
	}

};