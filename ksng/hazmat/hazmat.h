#include "../util/fileops.h"
#include <cstdlib>
#include <cstring>

using namespace std;

namespace hazmat {

	bool HAZMAT_ACCESS_ALLOWED = false;

	void hazmatWarning() {
		if (HAZMAT_ACCESS_ALLOWED) { return; }

		cout << colors::colorize("Kingslayer-NG: hazardous materials warning: hazmat was requested. Grant access? (y/n)", colors::WARNING);
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

	int execute(string name, string interpreter) {
		hazmatWarning();
		stringstream ss;
		ss << interpreter << "ksng/hazmat/hazmat/" << name;
		int result = system(ss.str().c_str());
		return result;
	}

	int executeProgram(string name) {
		return execute(name, (string)("./"));
	}

	int executeBash(string name) {
		return execute(name, (string)("bash "));
	}

	int executePython(string name) {
		return execute(name, (string)("python3 "));
	}

};