#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <random>

using namespace std;

const char SONG_SHORT[43] = "KINGSLAYER, DESTROYING CASTLES IN THE SKY!";
const char SONG_LONG[193] = "KINGSLAYER, DESTROYING CASTLES IN THE SKY!\nKINGSLAYER, FOREVERMORE THE APPLE OF MY EYE!\nI'D SACRIFICE MY LIFE TO FIND YOU, ANGEL OF THE BLADE!\nKINGSLAYER, COME AND COLLECT US FROM THE NIGHT!";

const string HEX_ALPHABET[] = {
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

class Bytestring {

	public:

		int length;
		unsigned char* data;

		Bytestring() {
			// Defaults to a single null byte

			length = 1;
			data = new unsigned char[1];
			data[0] = 0;
		}

		Bytestring(const int _length) {
			length = _length;
			data = new unsigned char[length];
			for (int i = 0; i < length; i++) {
				data[i] = 0;
			}
		}

		Bytestring(unsigned char _data[], const int _length) {
			length = _length;
			data = new unsigned char[length];
			for (int i = 0; i < length; i++) {
				data[i] = _data[i];
			}
		}

		Bytestring(Bytestring data1, Bytestring data2) {
			// bitwise xor

			if (data1.length < data2.length) {
				length = data1.length;
			} else {
				length = data2.length;
			}
			
			data = new unsigned char[length];

			for (int i = 0; i < length; i++) {
				data[i] = data1[i] ^ data2[i];
			}
		}

		void copyTo(Bytestring other, int offset=0) {
			for (int i = 0; i < length; i++) {
				if (i + offset >= other.length) {
					return;
				}
				other.data[i + offset] = data[i];
			}
		}

		Bytestring concatenate(Bytestring other) {
			const int totalLength = length + other.length;
			unsigned char placeholder[totalLength];
			Bytestring result = Bytestring(placeholder, totalLength);
			copyTo(result, 0);
			other.copyTo(result, length);
			return result;
		}

		Bytestring bitwiseXor(Bytestring other) {
			unsigned char xored[length];
			for (int i = 0; i < length; i++) {
				xored[i] = data[i] ^ other.data[i];
			}
			return Bytestring(xored, length);
		}

		Bytestring substring(int a, int b) {
			const int newLength = b - a;

			Bytestring result = Bytestring(newLength + 1);
			for (int i = 0; i < (b + 1); i++) {
				result.data[i] = data[a + i];
			}

			return result;
		}

		// operator overrides
		bool operator==(const Bytestring* other) const {
			if (length != other->length) {
				return false;
			}

			for (int i = 0; i < length; i++) {
				if (data[i] != other->data[i]) {
					return false;
				}
			}

			return true;
		}

		unsigned char operator[](const int index) {
			unsigned char d = data[index];
			return d;
		}

		void operator>>(Bytestring other) {
			copyTo(other, 0);
		}

		string hexadecimal(string delimiter) {
			string result;
			for (int i = 0; i < length; i++) {
				result += HEX_ALPHABET[data[i]];
				if (i != length - 1) {
					result += delimiter;
				}
			}
			return result;
		}

		string hexadecimal() {
			return hexadecimal("-");
		}

		unsigned short checksum() {
			unsigned int s;
			unsigned short c;
			for (int i = 0; i < length; i += 2) {
				c = (data[i] * 256) + data[i + 1];
				s += c;
			}
			unsigned short chk = 0;
			chk += (short)(s & 0xffff);
			chk += (short)(s % 65536);
			return chk;
		}

		unsigned short getShort(int index) {
			return ((unsigned short)(data[index]) * 256) + (unsigned short)(data[index + 1]);
		}

		unsigned int getInt(int index) {
			return ((unsigned int)(data[index]) * (unsigned int)(16777216));
			       + ((unsigned int)(data[index + 1]) * (unsigned int)(65536));
			       + ((unsigned int)(data[index + 2]) * (unsigned int)(256));
			       + (unsigned int)(data[index + 3]);
		}

		void erase() {
			for (int i = 0; i < length; i++) {
				for (int j = 0; j < 256; j++) {
					data[i] = 0;
					data[i] = 1;
					data[i] = rand() % 2;
				}
				data[i] = 0;
			}
			for (int i = 0; i < length; i++) {
				data[i] = SONG_LONG[i % 193];
			}
		}

};

class Bytestream {

	public:

		Bytestring streamData = Bytestring(0);

		Bytestream() {}

		void operator<<(Bytestring newData) {
			streamData = streamData.concatenate(newData);
		}

		Bytestring bytestring() {
			return streamData;
		}

};

string bytestringToIPv4(Bytestring data) {
	stringstream s;
	for (char i = 0; i < 4; i++) {
		s << (int)(data[i]);
		if (i < 3) {
			s << ".";
		}
	}
	return s.str();
}

void destroyData(void* ptr, size_t size) {
	for (int i = 0; i < 999; i++) {
		memset(ptr, 0, size);
		memset(ptr, 1, size);
		memset(ptr, rand() % 2, size);
	}
}

int getFileSize(const std::string& filename) {
	ifstream file(filename, ios::binary | ios::ate);
	if (!file.is_open()) {
		// Handle error: unable to open the file
		return -1;
	}

	int size = static_cast<int>(file.tellg());
	file.close();

	return size;
}

Bytestring readBytes(const string& filename, int byteCount) {
	Bytestring result(byteCount);
	ifstream file(filename);

	char chara = 0;
	int i = 0;
	if (file.is_open()) {
		while (!file.eof()) {
			file.get(chara);
			result.data[i] = static_cast<unsigned char>(chara);
			i++;
		}
	}

	return result;
}

inline Bytestring readFile(const string& filename) {
	int fileSize = getFileSize(filename);
	return readBytes(filename, fileSize);
}

int writeFile(string filename, Bytestring data) {
	ofstream file(filename, ios::binary | ios::trunc);
	if (!file.is_open()) {
		cerr << "fatal error: could not open file";
		return -1;
	}

	if (!file.good()) {
		cerr << "fatal error: could not write to file";
		return -2;
	}

	for (int i = 0; i < data.length; i++) {
		file << static_cast<char>(data[i]);
	}

	file.close();
	return 0;
}