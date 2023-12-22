#pragma once

#include <stdexcept>
#include <iostream>

using namespace std;

namespace colors {
	string HEADER = "\033[95m";
	string OKBLUE = "\033[94m";
	string OKCYAN = "\033[96m";
	string OKGREEN = "\033[92m";
	string WARNING = "\033[93m";
	string FAIL = "\033[91m";
	string ENDC = "\033[0m";
	string BOLD = "\033[1m";
	string UNDERLINE = "\033[4m";

	string colorize(string text, string color) {
		return color + text + ENDC;
	}
};

namespace notif {

	void fatal(string msg) {
		cerr << colors::colorize("\n\nKingslayer-NG: fatal system failure: " + msg + "\n\n", colors::FAIL + colors::BOLD) << endl;
		throw exception();
	}

	void error(string msg) {
		cerr << colors::colorize("\nKingslayer-NG: system failure: " + msg + "\n", colors::FAIL) << endl;
	}

	void warning(string msg) {
		cerr << colors::colorize("Kingslayer-NG: warning: " + msg, colors::WARNING) << endl;
	}

	void info(string msg) {
		cout << colors::colorize("Kingslayer-NG: " + msg, colors::OKCYAN) << endl;
	}

	void success(string msg) {
		cout << colors::colorize("Kingslayer-NG: success: " + msg, colors::OKGREEN) << endl;
	}

}