#pragma once
#include "../../util/fileops.h"
#include "csprng.h"

using namespace std;

namespace key {

	class Key {

		private:

			data::Bytes keybytes;
			data::Bits keybits;
			bool locked = false;

		public:

			// Constructors & destructors

			Key(data::Bytes data) {
				keybytes = data;
				keybits = data::bytesToBits(data);

				keybytes.hide();
				keybits.hide();
			}

			Key(string filename) {
				keybytes = fileops::readFileBytes(filename);
				keybits = fileops::readFileBits(filename);

				keybytes.hide();
				keybits.hide();
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() {
				if (locked) {
					notif::security("attempted access to locked cryptographic key", ALERT);
				}
			}

			// Access

			unsigned char getByte(int index) {
				securityCheck();
				keybytes.reveal();
				unsigned char result = keybytes.get(index);
				keybytes.hide();
				return result;
			}

			int getBit(int index) {
				securityCheck();
				keybits.reveal();
				int result = keybits.get(index);
				keybits.hide();
				return result;
			}

			// File management

			void store(string filename) {
				securityCheck();
				keybytes.reveal();
				fileops::writeFile(filename, keybytes);
				keybytes.hide();
			}

			// Utility

			static inline Key random(int bitlength) {
				return Key(csprng::bytes(bitlength / 8));
			}

			static inline Key random(int bytelength)

			static Key generate(int bitlength)

	};

};