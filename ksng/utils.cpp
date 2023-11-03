#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <bits/stdc++.h>

using namespace std;

const char* HEX_ALPHABET[16] = {
	"0", "1", "2", "3",
	"4", "5", "6", "7",
	"8", "9", "A", "B",
	"C", "D", "E", "F"
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

		void copyTo(Bytestring other, int offset=0) {
			for (int i = 0; i < length; i++) {
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
			if (newLength < 1) {
				return substring(b, a);
			}

			unsigned char placeholder[newLength];
			Bytestring result = Bytestring(placeholder, newLength);
			for (int i = a; i < b + 1; i++) {
				result.data[i] = data[i];
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

		void operator>>(Bytestring other) {
			copyTo(other, 0);
		}

};

Bytestring nullString(const int length) {
	unsigned char nullBytestring[length];
	return Bytestring(nullBytestring, length);
}