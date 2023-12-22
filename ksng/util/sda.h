#pragma once
#include <sstream>
#include <cstring>
#include "notif.h"

using namespace std;

namespace sda {

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
				if (index >= getLength() || index < 0) {
					notif::fatal("list index out of range trying to access data array");
					return elements[0];
				}
				return elements[index];
			}

			void set(int index, T value) {
				securityCheck();
				if (index >= getLength() || index < 0) {
					notif::fatal("list index out of range trying to access data array");
				}
				elements[index] = value;
			}

			int getLength() {
				securityCheck();
				return length;
			}

			// Utility

			string repr(string delimiter=(string)("-")) {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if (i != getLength() - 1) {
						ss << delimiter;
					}
				}
				return ss.str();
			}

	};

};