#pragma once
#include "dstruct/sda.h"
#include <ctime>
#include <random>

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

using namespace std;

namespace data {

	enum DataRepr {
		bits = 1,
		bytes = 8
	};

	class Bits: public sda::SecureDataArray<int> {

		public:

			explicit Bits() {}

			explicit Bits(int length_, bool locked_=false, Severity severityLevel_=ALERT) {
				initialize(length_, locked_, severityLevel_);
			}

			Bits(initializer_list<int> initList) {
				length = initList.size();
				elements = new int[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			Bits operator^(Bits other) {
				int length = getLength();
				Bits result(length);
				for (int i = 0; i < length; i++) {
					result.set(i, get(i) ^ other.get(i));
				}
				return result;
			}

			string bin() {
				stringstream ss;
				ss << getLength() << " bits: ";
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if ((i + 1) % 8 == 0 && i != getLength() - 1) {
						ss << "-";
					}
				}
				return ss.str();
			}

			Bits subbits(int a, int b) {
				if (b < a) {
					return subbits(b, a);
				} else if (a == b) {
					Bits result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					Bits result(b - a);
					for (int i = 0; i < b - a; i++) {
						if (a + i >= getLength()) { break; }
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
					return Bits(0);
				}
			}

			void copyTo(Bits &dst, int offset = 0) {
				for (int i = 0; i < getLength(); i++) {
					if (i + offset >= dst.getLength()) {
						break;
					}
					dst.set(i + offset, get(i));
				}
			}

			Bits copyOfSize(int i) {
				Bits result(i);
				copyTo(result);
				return result;
			}
			Bits extended(int i) { return copyOfSize(getLength() + i); }
			Bits truncated(int i) { return copyOfSize(getLength() - 1); }

	};

	class Bytes: public sda::SecureDataArray<unsigned char> {

		public:

			explicit Bytes() {}

			explicit Bytes(int length_, bool locked_=false, Severity severityLevel_=ALERT) {
				initialize(length_, locked_, severityLevel_);
			}

			explicit Bytes(string s) {
				initialize(s.length(), false, ALERT);
				for (int i = 0; i < s.length(); i++) {
					set(i, static_cast<unsigned char>(s[i]));
				}
			}

			Bytes(unsigned char* buf, int len) {
				initialize(len, false, ALERT);
				for (int i = 0; i < len; i++) { set(i, buf[i]); }
			}

			Bytes(initializer_list<unsigned char> initList) {
				length = initList.size();
				elements = new unsigned char[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			Bytes operator^(Bytes other) {
				Bytes result(getLength());
				for (int i = 0; i < getLength(); i++) {
					result.set(i, get(i) ^ other.get(i));
				}
				return result;
			}

			// ~Bytes() { erase(); }

			string hex() {
				stringstream ss;
				ss << getLength() << " bytes: ";
				for (int i = 0; i < getLength(); i++) {
					ss << HEX_ALPHABET[get(i)];
					if (i != getLength() - 1) {
						ss << "-";
					}
				}
				return ss.str();
			}

			Bytes subbytes(int a, int b) {
				if (b < a) {
					return subbytes(b, a);
				} else if (a == b) {
					Bytes result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					Bytes result(b - a);
					for (int i = 0; i < b - a; i++) {
						if (a + i >= getLength()) { break; }
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
					return Bytes(0);
				}
			}

			// Copying and modification

			void copyTo(Bytes &dst, int offset = 0) {
				for (int i = 0; i < getLength() && i + offset < dst.getLength(); i++) {
					dst.set(i + offset, get(i));
				}
			}

			Bytes copyOfSize(int i) {
				Bytes result(i);
				copyTo(result);
				return result;
			}
			Bytes extended(int i) { return copyOfSize(getLength() + i); }
			Bytes truncated(int i) { return copyOfSize(getLength() - 1); }

			string astext() {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
				}
				return ss.str();
			}

			unsigned short getShort(int index) {
				return ((unsigned short)(get(index)) * 256) + (unsigned short)(get(index + 1));
			}

			void loadShort(unsigned short value, int index=0) {
				set(index, value >> 8);
				set(index + 1, value % 256);
			}

			unsigned short getInt(int index) {
				return ((unsigned int)(get(index)) << 24) \
					   + ((unsigned int)(get(index + 1)) << 16) \
					   + ((unsigned int)(get(index + 2)) << 8) \
					   + (unsigned int)(get(index + 3));
			}

			void loadInt(unsigned int value, int index=0) {
				set(index, value >> 24);
				set(index + 1, (value >> 16) % 256);
				set(index + 2, (value >> 8) % 256);
				set(index + 3, value % 256);
			}

			Bytes operator+(Bytes other) {
				Bytes result(getLength() + other.getLength());
				copyTo(result);
				other.copyTo(result, getLength());
				return result;
			}

			void erase() {
				srand(time(nullptr));
				for (int i = 0; i < 256; i++) {
					for (int j = 0; j < getLength(); j++) {
						set(j, rand() % 256);
					}
				}
				for (int i = 0; i < getLength(); i++) {
					set(i, 0);
				}
			}

	};

	class Bytestream {

		private:
		
			Bytes s;

		public:

			Bytestream() {
				s.initialize(0);
			}

			void operator<<(Bytes d) {
				Bytes n(s.getLength() + d.getLength());
				s.copyTo(n, 0);
				d.copyTo(n, s.getLength());
				s = n;
			}

			Bytes bytes() {
				return s;
			}

	};

	Bits bytesToBits(Bytes bytes) {
		Bits result(bytes.getLength() * 8);
		unsigned char byte;
		for (int i = 0; i < bytes.getLength(); i++) {
			byte = bytes.get(i);
			for (int j = 0; j < 8; j++) {
				result.set((i * 8) + j, (byte >> (8 - (j + 1))) % 2);
			}
		}
		return result;
	}

	Bytes bitsToBytes(Bits bits) {
		if (bits.getLength() % 8 != 0) { 
			notif::error("truncating bits so that byte conversion works"); 
			bits = bits.truncated(bits.getLength() % 8);
		}

		Bytes result(bits.getLength() / 8);
		unsigned char x;
		int bitOffset;

		for (int byteOffset = 0; byteOffset < bits.getLength() / 8; byteOffset++) {
			bitOffset = byteOffset * 8;
			x = 0;

			for (int i = 0; i < 8; i++) {
				x += bits.get(bitOffset + i) << (8 - (i + 1));
			}

			result.set(byteOffset, x);
		}

		return result;
	}

};