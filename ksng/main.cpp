#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <chrono>

#include "hazmat/hazmat.h"
#include "net/pktd.h"
#include "hack/probe.h"
#include "hack/incision.h"
#include "crypto/core/key.h"
#include "crypto/ciphers/vox.h"

using namespace std;

std::string cmd[256];

int cmdlength;

bool cmdContains(std::string s) { for (int i = 0; i < 256; i++) { if (cmd[i] == s) { return true; } } return false; }

bool EXIT = false;

int processCommand(int i) {

	//// Process command ////

	// command

	if (cmd[0] == "exit") { 
		cout << "Exiting." << endl;
		EXIT = true;
		return -1; 
	} else if (cmd[0] == "help") {
		cout << "Currently available (and documented) commands:" << endl;
		cout << "  clear        - clear the screen" << endl;
		cout << "  runtest      - a.k.a. rt, run a test file" << endl;
		cout << "  execute      - a.k.a. exec, ex, x, execute a hazmat file" << endl;
		cout << "  header       - a.k.a. hdr, manage header files" << endl;
		cout << "  oscmd        - a.k.a. os, $, /, run an OS command (WIP)" << endl;
		cout << "  reload       - a.k.a. rl, reload the shell (if main.cpp has been updated)" << endl;
		cout << "  pythonshell  - a.k.a. pysh, open a Python shell" << endl;
		cout << "  hack         - a.k.a. h, initiate a hack of some variety" << endl;
		cout << "  cryptography - a.k.a. crypto, c, perform a cryptographic operation" << endl;
		cout << "  exit         - exit the shell" << endl;
		cout << "Currently available universal flags:" << endl;
		cout << "  --sudo       - execute shell commands as superuser" << endl;
		cout << "  --thenclear  - once the command is done, clear the screen" << endl;
		cout << "  --thenexit   - once the command is done, exit the shell" << endl;
	} else if (cmd[0] == "clear") {
		cout << "\033[2J\033[1;1H";
	} else if (cmd[0] == "rt" || cmd[0] == "runtest") {
		stringstream stcm;
		if (cmdContains("--sudo")) {
			stcm << "sudo ";
		}
		stcm << "bash ksng/cmpltst.bash " << cmd[1];
		system(stcm.str().c_str());
	} else if (cmd[0] == "exec" || cmd[0] == "ex" || cmd[0] == "x" || cmd[0] == "execute") {
		if (cmd[1] == "py" || cmd[1] == "python") {
			hazmat::executePython(cmd[2]);
		} else if (cmd[1] == "bash") {
			hazmat::executeBash(cmd[2]);
		} else if (cmd[1] == "f") {
			hazmat::executeProgram(cmd[2]);
		} else {
			cout << "Unknown interpreter." << endl;
		}
	} else if (cmd[0] == "header" || cmd[0] == "hdr") {
		stringstream stcm;
		if (cmdContains("--sudo")) {
			stcm << "sudo ";
		}
		stcm << "bash ksng/";
		if (cmd[1] == "cmpl" || cmd[1] == "compile" || cmd[1] == "c") { 
			stcm << "cmplhdr.bash " << cmd[2]; 
		} else if (cmd[1] == "rm" || cmd[1] == "remove" || cmd[1] == "r") {
			stcm << "rmgch.bash " << cmd[2];
		} else if (cmd[1] == "test" || cmd[1] == "tst" || cmd[1] == "t") {
			stcm << "cmplhdr.bash " << cmd[2] << ";ksng/rmgch.bash " << cmd[2];
		} else if (cmd[1] == "lc" || cmd[2] == "linecount") {
			system("find ksng -name '*.h' | xargs wc -l");
			return 0;
		}
		system(stcm.str().c_str());
	} else if (cmd[0] == "os" || cmd[0] == "oscmd" || cmd[0] == "$" || cmd[0] == "/") {
		stringstream ss;
		for (int i = 1; i < cmdlength && cmd[i] != (string)(" "); i++) {
			ss << " " << cmd[i];
		}
		cout << "Attempting OS command ..." << endl << endl;
		int rv = system(ss.str().c_str());
		if (rv == 0) { cout << endl << "Execution successful." << endl; } else { cout << "Exit code: " << rv << endl; }
		for (i = i + 1; i < 256; i++) {
			cmd[i] = (string)(" ");
			cout << cmd[i];
		}
	} else if (cmd[0] == "reload" || cmd[0] == "rl") {
		cout << "Reloading Kingslayer." << endl << "Recompiling ksng/main.cpp ..." << endl;
		int rv = system("g++ ksng/main.cpp -o kingslayer");
		if (rv == 0) {
			cout << "Recompile successful." << endl << "Warning: recompiling many times may lead to degraded performance, at which point a manual restart is suggested."
					<< endl << "Reloading Kingslayer ..." << endl;
			system("./kingslayer");
			return 0;
		} else {
			cout << "Recompile error, could not reload." << endl;
		}
	} else if (cmd[0] == "pythonshell" || cmd[0] == "pyshell" || cmd[0] == "pythonsh" || cmd[0] == "pysh") {
		cout << "Loading Python shell." << endl;
		int rv = system("python3");
		cout << "Return code: " << rv << endl;
	} else if (cmd[0] == "hack" || cmd[0] == "h") {
		if (cmd[1] == "spy") {
			if (cmd[2] == "" || cmd[2] == " " || cmd[3] == "" || cmd[3] == " ") {
				cout << "Improperly formatted command; format: \"hack spy <interface> <packets> [--lm]\"" << endl;\
				return 0;
			}
			pktd::PacketDissector dsctr(cmd[2]);
			cout << "Initializing spying.\n  Interface name: " << cmd[2] << endl;
			cout << "  Desired number of packets: " << cmd[3] << " packets";
			cout << endl;
			cout << "  Interface connection: " << colors::colorize("ARMED", colors::OKGREEN) << endl;
			data::Bytes rawData;
			bool go = false;
			if (cmdContains("--lm")) { go = true; } else { go = notif::confirm(); }
			if (go) {
				cout << "Spying on " << cmd[2] << " ..." << endl;

				int i = stoi(cmd[3]);
				while (i --> 0) {
					cout << dsctr.receivePacket().repr() << endl;
				}
			}
			cout << "Spying operation complete." << endl;
		} else {
			notif::error("Unknown or invalid hack type.");
		}
	} else if (cmd[0] == "crypto" || cmd[0] == "cryptography" || cmd[0] == "c") {
		if (cmd[1] == "" || cmd[1] == " ") {
			notif::error("Invalid cryptographic operation. Proper syntax: \"crypto [[[encrypt/decrypt] <system> <filename> <keyname>]/[gk <system> <name>]]\"");
			return 0;
		}
		if (cmd[1] == "gk" || cmd[1] == "generatekey" || cmd[1] == "generate") {
			if (cmd[2] == "" || cmd[2] == " " || cmd[3] == "" || cmd[3] == " ") {
				notif::error("syntax error: key generation must be accompanied with a cryptographic system name and a filename");
				return 0;
			}
			cout << "Loading key generation algorithm ...";
			key::Key k;
			if (cmd[2] == "vox") {
				vox::VOX cs;
				cout << "done." << endl;
				k = cs.generateKey();
				cout << colors::colorize("Successfully generated Vector Operation Interchange key.", colors::OKCYAN) << endl;
			} else {
				cout << colors::colorize("Invalid cryptosystem. Cannot generate key.", colors::FAIL) << endl;
			}
			cout << "  Raw key data:\n\n    " << k.getBytes().hex() << "\n\n";
			cout << "Storing key ...";
			k.store(cmd[3] + ".key");
			cout << colors::colorize("successfully stored key in file.", colors::OKCYAN) << endl;
			cout << "Checking key integrity ..." << endl;
			cout << "  Loading new key from file ...";
			key::Key nk(cmd[3] + ".key");
			cout << "done.\n  Cross-referencing keys ...";
			bool good = k == nk;
			cout << "done.\n"; 
			if (good) {
				cout << colors::colorize("Key integrity confirmed.", colors::OKCYAN) << endl;
			} else {
				notif::security("key integrity failure detected.", ALERT);
			}
		}
		if (cmd[1] == "encrypt" || cmd[1] == "e") {
			cout << "Loading ";
		}
	} else {
		cout << "Unknown or invalid command." << endl;
	}

	// post-command

	if (cmdContains("--thenclear")) {
		cout << "\033[2J\033[1;1H";
	}
	if (cmdContains("--thenexit")) {
		cout << "Exiting." << endl;
		EXIT = true;
		return -1;
	}
	return 0;
}

int main(int argc, char** argv) {

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			cmd[i - 1] = argv[i];
		}
		processCommand(argc - 1);
		exit(0);
	}

	cout << "\033[2J\033[1;1H";

	cout << "K1N95L4Y3R: N3XT 63N!" << endl;
	cout << "Enter any command to continue, and use \"exit\" or [Ctrl]+[C] to exit." << endl;

	while (true) {
		cout << endl;
		//// Read input from the user ////
		std::string s;
		cout << " > ";
		getline(cin, s);
		s = s + (string)(" "); // ensure delimiter works right
		std::string delimiter = " ";

		size_t pos = 0;
		std::string token;
		int i = 0;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			cmd[i] = token;
			i++;
			s.erase(0, pos + delimiter.length());
		}
		cmdlength = i;
		for (i = i + 1; i < 256; i++) {
			cmd[i] = (string)(" ");
		}

		cout << endl;
		if (processCommand(i) == -1) { exit(0); };
		for (i = i + 1; i < 256; i++) {
			cmd[i] = (string)(" ");
			cout << cmd[i];
		}

		if (EXIT) { break; }
	}

	return 0;

}