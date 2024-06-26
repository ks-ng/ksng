#pragma once
#include <stdexcept>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

void hideCursor() {
	cout << "\e[?25l";
}

void showCursor() {
	cout << "\e[?25h";
}

void sleep(int us) {
    usleep(us);
}

string removeBeforeSymbol(const string &str, const string &symbol) {
    size_t pos = str.find(symbol);
    if (pos != std::string::npos) {
        return str.substr(pos + symbol.length());
    }
    return str; // Return the original string if the symbol is not found
}

string getBeforeSymbol(const string &str, const string &symbol) {
	size_t pos = str.find(symbol);
    if (pos != std::string::npos) {
        return str.substr(0,pos);
    }
    return str; // Return the original string if the symbol is not found
}

enum Severity {
	CAUTION = 1,
	WARNING = 2,
	ALERT = 3,
};

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

	inline void fatal(string msg) {
		cerr << colors::colorize("\n\nKingslayer-NG: fatal system failure: " + msg + "\n\n", colors::FAIL + colors::BOLD) << endl;
		throw exception();
	}

	inline void error(string msg) {
		cerr << colors::colorize("\nKingslayer-NG: system failure: " + msg + "\n", colors::FAIL) << endl;
	}

	inline void warning(string msg) {
		cerr << colors::colorize("Kingslayer-NG: warning: " + msg, colors::WARNING) << endl;
	}

	inline void info(string msg) {
		cout << colors::colorize("Kingslayer-NG: " + msg, colors::OKCYAN) << endl;
	}

	inline void success(string msg) {
		cout << colors::colorize("Kingslayer-NG: success: " + msg, colors::OKGREEN) << endl;
	}

	void security(string msg, Severity severity) {
		if (severity == CAUTION) {
			cout << colors::colorize("Kingslayer-NG: security advises ", colors::HEADER) \
					+ colors::colorize("caution", colors::WARNING + colors::UNDERLINE + colors::BOLD) \
					+ colors::colorize(": " + msg, colors::HEADER);
		} else if (severity == WARNING) {
			cout << colors::colorize("Kingslayer-NG: ", colors::HEADER) \
					+ colors::colorize("security warning", colors::FAIL + colors::UNDERLINE + colors::BOLD) \
					+ colors::colorize(": " + msg, colors::HEADER);
		} else if (severity == ALERT) {
			cout << colors::colorize("\nKingslayer-NG: ", colors::HEADER) \
					+ colors::colorize("SECURITY ALERT", colors::FAIL + colors::UNDERLINE + colors::BOLD) \
					+ colors::colorize(": " + msg + "\n", colors::HEADER);
		} else {
			fatal("security system failure");
		}
	}


	bool confirm() {
		cout << "Confirm? (y/n)";
		string usr;
		while (true) {
			cin >> usr;
			if (usr == (string)("y")) {
				cout << "Action confirmed." << endl;
				return true;
			} else if (usr == (string)("n")) {
				cout << "Action cancelled.";
				return false;
			} else {
				cout << "Please input \"y\" or \"n\".";
			}
		}
		return false; // sigh
	}

};