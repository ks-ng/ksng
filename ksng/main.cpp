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

	while (true) {
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

		//// Process command ////

		// command

		if (cmd[0] == "exit") { 
			break; 
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
		} else {
			cout << "Unknown or invalid command." << endl;
		}

		// post-command

		if (cmdContains("--thenexit")) {
			break;
		}

	}

}