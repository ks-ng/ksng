#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#include "hazmat/hazmat.h"

using namespace std;

std::string cmd[256];

bool cmdContains(std::string s) { for (int i = 0; i < 256; i++) { if (cmd[i] == s) { return true; } } return false; }

int main() {
	cout << "K1N95L4Y3R: N3XT 63N!" << endl;
	cout << "Enter any command to continue." << endl;

	int cmdlength;

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

		//// Process command ////

		// command

		if (cmd[0] == "exit") { 
			break; 
		} else if (cmd[0] == "help") {
			cout << "Currently available (and documented) commands:" << endl;
			cout << "  clear       - clear the screen" << endl;
			cout << "  runtest     - a.k.a. rt, run a test file" << endl;
			cout << "  execute     - a.k.a. exec, ex, x, execute a hazmat file" << endl;
			cout << "  header      - a.k.a. hdr, manage header files" << endl;
			cout << "  oscmd       - a.k.a. os, $, /, run an OS command (WIP)" << endl;
			cout << "  reload      - a.k.a. rl, reload the shell (if main.cpp has been updated)" << endl;
			cout << "  exit        - exit the shell" << endl;
			cout << "Currently available universal flags:" << endl;
			cout << "  --sudo      - execute shell commands as superuser" << endl;
			cout << "  --thenclear - once the command is done, clear the screen" << endl;
			cout << "  --thenexit  - once the command is done, exit the shell" << endl;
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
				continue;
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
				break;
			} else {
				cout << "Recompile error, could not reload." << endl;
			}
		} else {
			cout << "Unknown or invalid command." << endl;
		}

		// post-command

		if (cmdContains("--thenclear")) {
			cout << "\033[2J\033[1;1H"; // command clears the screen
		}
		if (cmdContains("--thenexit")) {
			break;
		}

	}

	return 0;

}