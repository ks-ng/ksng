#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;

const string HEX_ALPHABET[256] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
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

	void fatal(string msg) {
		cout << colors::colorize("\n\nKingslayer-NG: fatal system failure: " + msg + "\n\n", colors::FAIL + colors::BOLD) << endl;
		throw exception();
	}

	void error(string msg) {
		cout << colors::colorize("\n\nKingslayer-NG: system failure: " + msg + "\n\n", colors::FAIL) << endl;
	}

	void warning(string msg) {
		cout << colors::colorize("Kingslayer-NG: warning: " + msg, colors::WARNING) << endl;
	}

	void info(string msg) {
		cout << colors::colorize("Kingslayer-NG: " + msg, colors::OKCYAN) << endl;
	}

	void success(string msg) {
		cout << colors::colorize("Kingslayer-NG: success: " + msg, colors::OKGREEN) << endl;
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

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }

			void erase() {
				for (int i = 0; i < 256; i++) {
					memset(data, 255, bytelength);
					memset(data, 0, bytelength);
				}
				notif::info("successfully erased data");
			}

			void accessSecurityCheck() {
				if (locked) {
					notif::error((string)("security failure: privatized data had invalid access attempt"));
				}
			}

			// Reading

			int length(Mode mode=bytes) {
				accessSecurityCheck();
				if (mode == 8) {
					return bytelength;
				} else if (mode == 1) {
					return bytelength * 8;
				} else {
					notif::fatal("bad length request type; only supports bits and bytes");
					return 0;
				}
			}

			unsigned char getBit(int bitOffset) { 
				accessSecurityCheck();
				if (bitOffset >= bytelength * 8) {
					notif::error("bit offset out of range");
					return 0;
				}
				return ((data[(int)(bitOffset / 8)]) >> bitOffset) % 2; 
			}
			unsigned char getByte(int byteOffset) { 
				accessSecurityCheck(); 
				if (byteOffset >= bytelength) {
					notif::error("byte offset out of range");
					return 0;
				}
				return data[byteOffset]; 
			}
			
			template <typename T> 
			make_unsigned<T> getNum(int offset, int bitLength, Mode offsetType=bytes) {
				accessSecurityCheck();
				make_unsigned<T> result = 0;
				if (offsetType == bits) {
					for (int i = 0; i < bitLength; i++) {
						result += getBit(offset + i) * pow(2, (bitLength - i) - 1);
					}
				} else if (offsetType == bytes) {
					for (int i = 0; i < bitLength; i++) {
						result += getBit((offset * 8) + i) * pow(2, (bitLength - i) - 1);
					}
				}
				return result;
			}

			// Writing

			void setByte(int byteOffset, unsigned char byte) {
				accessSecurityCheck();
				if (byteOffset >= length(bytes)) {
					notif::error("byte offset out of range");
				} else {
					data[byteOffset] = byte;
				}
			}

			// Utilities

			string hex(string delimiter=(string)("-")) {
				stringstream ss;
				for (int i = 0; i < length(bytes); i++) {
					ss << HEX_ALPHABET[getByte(i)];
					if (i < bytelength - 1) {
						ss << delimiter;
					}
				}
				return ss.str();
			}

	};

};