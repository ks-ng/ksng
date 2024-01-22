#include "util/sda.h"

#define MAX_COMMAND_ARGS 20

using namespace std;

void startPrompt() {
	cout << endl << " ╔═════ K1N95L4Y3R: N3X7 63N! ═════╗ " << endl;
	cout <<         " ╚═ destroying castles in the sky ═╝" << endl << endl;
}

void endPrompt() {
	cout << endl << endl << " ╔ K1N95L4Y3R, C0M3 4ND C0113C7 U5 FR0M 7H3 N19H7! ╗ " << endl;
	cout <<                 " ╚═════════════════════════════════════════════════╝ " << endl << endl;
}

void cli() {
	startPrompt();
	std::string user;
	stringstream arg;
	sda::SDA<std::string> args;
	int argidx = 0;
	while (true) {
		cout << " > ";
		std::getline(std::cin, user);
		if (user == "exit") {
			break;
		}
		argidx = 0;
		for (int i = 0; i < user.length(); i++) {
			if (user.at(i) == (string)(" ")) {
				args.set(argidx, arg.str());
				argidx++;
			} else {
				arg << user.at(i);
			}
		}
	}
	endPrompt();
}

int main() {
	cli();
}
