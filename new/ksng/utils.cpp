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

using namespace std;

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

			return result
		}

};

Bytestring nullString(const int length) {
	unsigned char nullBytestring[length];
	return Bytestring(nullBytestring, length);
}