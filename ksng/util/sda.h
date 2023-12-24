#pragma once
#include <sstream>
#include <cstring>
#include "notif.h"

using namespace std;

namespace sda {

	bool DISALLOW_OUT_OF_RANGE = true;

	template <typename T>
	class SecureDataArray {

		private:

			T* elements;
			int length;
			bool locked = false;
			Severity securityLevel;

		public:

			// Constructors & destructors

			SecureDataArray() {}
			SecureDataArray(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			void initialize(int length_, bool locked_=false, Severity securityLevel_=ALERT) { 
				length = length_;
				elements = new T[length];
				locked = locked_;
				securityLevel = securityLevel_;
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure data array", securityLevel);
				} 
			}

			// Access

			T get(int index) {
				securityCheck();
				if ((index >= getLength() || index < 0) && DISALLOW_OUT_OF_RANGE) {
					notif::fatal("index out of range trying to get item from data array (segfault prevented)");
					return elements[0];
				}
				return elements[index];
			}

			void set(int index, T value) {
				securityCheck();
				if (index >= getLength() || index < 0) {
					notif::fatal("index out of range trying to set item of data array (segfault prevented)");
				}
				elements[index] = value;
			}

			int getLength() {
				securityCheck();
				return length;
			}

			// Utility

			string repr(string delimiter=(string)(" ")) {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if (i != getLength() - 1) {
						ss << delimiter;
					}
				}
				return ss.str();
			}

			SecureDataArray<T> subarray(int a, int b) {
				if (b < a) {
					return subarray(b, a);
				} else if (a == b) {
					SecureDataArray<T> result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					SecureDataArray<T> result(b - a);
					for (int i = 0; i < b - a; i++) {
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
				}
			}

	};

	template <typename T>
	using SDA = SecureDataArray<T>;

};