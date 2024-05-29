#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <locale>

#include "hazmat/hazmat.h"
#include "net/pktd.h"
#include "hack/probe.h"
#include "hack/incision.h"
#include "crypto/core/key.h"
// #include "crypto/hashes/md.h" - deprecated in OpenSSL 3.0
#include "crypto/hashes/sha.h"
#include "crypto/ciphers/vox.h"

using namespace std;

std::string cmd[256];

int cmdlength;

inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

bool cmdContains(std::string s) { for (int i = 0; i < 256; i++) { if (cmd[i] == s) { return true; } } return false; }
string getStringArg(string flag, string allowedErrorValue=(string)("NONE")) {
	stringstream ss;
	int i = 0;
	bool found = false;
	for (i = 0; i < 256; i++) {
		if (cmd[i] == flag) {
			i++;
			found = true;
			for (int j = i; j < 250; j++) {
				if (cmd[j].substr(0,1) == string("-")) { break; }
				ss << cmd[j];
				if (cmd[j+1].substr(0,1) != string("-")) { ss << " "; }
			}
		}
	}
	if (!found) { return allowedErrorValue; }
	string result = ss.str();
	rtrim(result);
	return result;
}
int getIntArg(string flag, int allowedErrorValue=-1) {
	for (int i = 0; i < 256; i++) {
		if (cmd[i] == flag) {
			return stoi(cmd[i + 1]);
		}
	}
	return allowedErrorValue;
}

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
		if (cmd[1] == "runtest") {
			cout << endl;
			cout << "  --- runtest --- " << endl;
			cout << "Syntax:" << endl;
			cout << endl << "  runtest <filename>" << endl << endl;
			cout << "Synopsis:" << endl;
			cout << "  Runs the file with the name \".../ksng/tsts/tst_<filename>.cpp\"." << endl;
			cout << "  The file is compiled using G++ using the -lssl and -lcrypto flags" << endl;
			cout << "  in order to be immediately run before it is deleted. If there is " << endl;
			cout << "  a compilation error, the program will not be run and the culprit " << endl;
			cout << "  error will be displayed." << endl;
		} else if (cmd[1] == "execute") {
			cout << endl;
			cout << "  --- execute --- " << endl;
			cout << "Syntax:" << endl;
			cout << endl << "  execute <interpreter> <filename>" << endl << endl;
			cout << "Synopsis:" << endl;
			cout << "  Executes the file with the name \".../ksng/hazmat/hazmat/<filename>\"." << endl;
			cout << "  As opposed to test files, this command uses a selected interpreter to " << endl;
			cout << "  run the file. If <interpreter> is \"py\", then Python will be used; if" << endl;
			cout << "  it is \"bash\", then bash will be used; if it is \"f\", then the file " << endl;
			cout << "  is assumed to be executable and is run." << endl << endl;
			cout << "  The first time this command is per Kingslayer shell, a hazmat request " << endl;
			cout << "  will ask for user confirmation." << endl;
		} else if (cmd[1] == "cryptography") {
			cout << endl;
			cout << "  --- cryptography --- " << endl;
			cout << "Syntax:" << endl << endl;
			cout << "  cryptography encrypt -s <cryptosystem> -k <key> -f <file>" << endl;
			cout << "  cryptography decrypt -s <cryptosystem> -k <key> -f <file>" << endl;
			cout << "  cryptography hash -s <algorithm> -f <file>" << endl << endl;
			cout << "Synopsis:" << endl;
			cout << "  Performs the corresponding cryptographic operation (see syntax)." << endl;
		}
		return 0;
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
		int rv = system("g++ ksng/main.cpp -o kingslayer -lssl -lcrypto");
		if (rv == 0) {
			cout << "Recompile successful." << endl << "Warning: recompiling many times may lead to degraded performance, at which point a manual restart is suggested."
					<< endl << "Reloading Kingslayer ..." << endl;
			system("./kingslayer");
			EXIT = true;
			return -1;
		} else {
			cout << "Recompile error, could not reload." << endl;
		}
	} else if (cmd[0] == "pythonshell" || cmd[0] == "pyshell" || cmd[0] == "pythonsh" || cmd[0] == "pysh") {
		cout << "Loading Python shell." << endl;
		int rv = system("python3");
		cout << "Return code: " << rv << endl;
	} else if (cmd[0] == "hack" || cmd[0] == "h") {
		if (cmd[1] == "spy") {
			string interface = getStringArg("-i");
			int pktCount = getIntArg("-c");
			if (interface == "NONE" || pktCount == -1) {
				cout << "Improperly formatted command; format: \"hack spy -i <interface> -c <packets> [--lm]\"" << endl;
				return 0;
			}
			probe::spy(interface, pktCount, cmdContains("--lm"));
		} else if (cmd[1] == "udp") {
			string target = getStringArg("-t");
			int count = getIntArg("-c");
			int port = getIntArg("-p");
			if (target == "NONE") {
				cout << "No target selected." << endl;
				return 0;
			}
			if (port == -1) { port = 9999; }
			if (count == -1) { count = 1000000; }
			incision::DirectUDPFlood atk(target, port, incision::defaultPayload, cmdContains("--lm"), cmdContains("--hp"));
			atk.attack();
		} else if (cmd[1] == "udpanon") {
			string target = getStringArg("-t");
			int count = getIntArg("-c");
			int port = getIntArg("-p");
			if (target == "NONE") {
				cout << "No target selected." << endl;
				return 0;
			}
			if (port == -1) { port = 9999; }
			if (count == -1) { count = 1000000; }
			incision::AnonymousUDPFlood atk(target, port, incision::defaultPayload, cmdContains("--lm"), cmdContains("--hp"));
			atk.attack();
		} else {
			notif::error("Unknown or invalid hack type.");
		}
	} else if (cmd[0] == "crypto" || cmd[0] == "cryptography" || cmd[0] == "c") {
		string csa = getStringArg("-s");
		string ka = getStringArg("-k");
		string fa = getStringArg("-f");
		cout << "Cryptosystem selected: \"" << csa << "\"" << endl;
		cout << "Key file selected: \"" << ka << "\"" << endl;
		cout << "Target file selected: \"" << fa << "\"" << endl;
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
		} else if (cmd[1] == "encrypt" || cmd[1] == "en" || cmd[1] == "e") {
			if (csa == "vox") {
				cout << "Loading key ...";
				key::Key k(ka);
				cout << "done.\nLoading encryption algorithm ...";
				vox::VOX cs;
				cout << "done.\nReading file ...";
				data::Bytes pt = fileops::readFileBytes(fa);
				cout << "done.\nEncrypting ...\n  File size: " << pt.getLength() << " bytes" << endl;
				cout << "  Running Vector Operation Interchange algorithm ...";
				data::Bytes ct = cs.encrypt(pt, k);
				cout << "done.\n  Writing file ...";
				fileops::writeFile(fa, ct);
				cout << "done.\nSuccessfully encrypted file." << endl;
			} else {
				notif::error("Invalid cryptosystem.");
				return 0;
			}
		} else if (cmd[1] == "decrypt" || cmd[1] == "de" || cmd[1] == "d") {
			if (csa == "vox") {
				cout << "Loading key ...";
				key::Key k(ka);
				cout << "done.\nLoading decryption algorithm ...";
				vox::VOX cs;
				cout << "done.\nReading file ...";
				data::Bytes ct = fileops::readFileBytes(fa);
				cout << "done.\nDecrypting ...\n  File size: " << ct.getLength() << " bytes" << endl;
				cout << "  Running Vector Operation Interchange algorithm ...";
				data::Bytes pt = cs.decrypt(ct, k);
				cout << "done.\n  Writing file ...";
				fileops::writeFile(fa, pt);
				cout << "done.\nSuccessfully decrypted file." << endl;
			} else {
				notif::error("Invalid cryptosystem.");
				return 0;
			}
		} else if (cmd[1] == "hash" || cmd[1] == "h") {
			string sa = getStringArg("-p");
			cout << "Selected storage file: " << sa << endl;
			if (csa == "sha256") {
				cout << "Loading hashing function ...";
				sha::SHA_256 hf;
				stringstream ss;
				ss << "Generated hash: " << hf.hashFile(fa, sa).hex();
				cout << "done." << endl << colors::colorize(ss.str(), colors::BOLD + colors::OKCYAN) << endl;
				if (sa != "NONE") {
					cout << "Hash stored in " << sa;
				}
			}
		} else {
			notif::error("Invalid cryptographic operation. See \"help cryptography\" for syntax help.");
			return 0;
		}
	} else if (cmd[0] == "compile" || cmd[0] == "c") {
		if (cmd[1] == "cpp")
	} else if (cmd[0] == "tf") {
		cout << "-a=" << getIntArg("-a") << endl;
		cout << "-b=" << getIntArg("-b") << endl;
		cout << "-c=" << getStringArg("-c") << endl;
		return 0;
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