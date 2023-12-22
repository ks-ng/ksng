#pragma once
#include "../../util/fileops.h"

namespace key {

	class Key {

		private:

			data::Bytes keybytes;
			data::Bits keybits;
			bool locked = false;

		public:

			// Constructors & destructors

			Key(string filename) {
				keybytes = readFileBytes(filename);
				keybits = readFileBits(filename);

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

	};

};