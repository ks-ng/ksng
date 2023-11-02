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

		void printHex() {
			const int hexStringLength = (2 * length);
			const char* hexadecimal[hexStringLength];
			unsigned char byte;
			unsigned char first;
			unsigned char last;

			for (int i = 0; i < length; i++) {
				byte = data[i];
				first = (byte & 0xF0) >> 4;
				last = (byte & 0x0F);

				cout << HEX_ALPHABET[first];
				cout << HEX_ALPHABET[last];
			}
		}

};

Bytestring nullString(const int length) {
	unsigned char nullBytestring[length];
	return Bytestring(nullBytestring, length);
}