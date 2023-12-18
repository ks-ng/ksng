#include <string>
#include <iostream>
#include <cmath>

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

namespace sysfail {

	void fatal(string msg) {
		cout << colors::colorize("\n\nKingslayer-NG: fatal system failure: " + msg + "\n\n", colors::FAIL + colors::BOLD) << endl;
		throw exception();
	}

	void error(string msg) {
		cout << colors::colorize("\n\nKingslayer-NG: system failure: " + msg + "\n\n", colors::FAIL) << endl;
	}

	void warnings(string msg) {
		cout << colors::colorize("Kingslayer-NG: warning: " + msg, colors::WARNING) << endl;
	}

}

namespace data {

	enum Mode {
		bits = 1,
		bytes = 8
	};

	class Data {

		private:

			unsigned char* data;
			int bytelength;
			bool locked;

		public:

			Data(int length, Mode mode, bool locked): locked(locked) {
				if (mode == 1) {
					while (length % 8 != 0) {
						length++;
					}
					length = (int)(length / 8);
				}

				data = new unsigned char[(const int)(length)];
				bytelength = length;
			}

			void accessSecurityCheck() {
				if (locked) {
					sysfail::error((string)("security failure: privatized data had invalid access attempt"));
				}
			}

			unsigned char bit(int bitOffset) { accessSecurityCheck(); return ((data[(int)(bitOffset / 8)]) >> bitOffset) % 2; }
			unsigned char byte(int byteOffset) { accessSecurityCheck(); return data[byteOffset]; }
			
			template <typename T> 
			make_unsigned<T> num(int offset, int bitLength, Mode offsetType=bytes) {
				make_unsigned<T> result = 0;
				if (offsetType == bits) {
					for (int i = 0; i < bitLength; i++) {
						result += bit(offset + i) * pow(2, (bitLength - i) - 1);
					}
				} else if (offsetType == bytes) {
					for (int i = 0; i < bitLength; i++) {
						result += bit((offset * 8) + i) * pow(2, (bitLength - i) - 1);
					}
				}
				return result;
			}

	};

};