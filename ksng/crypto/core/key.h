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

			int getByteLength() { 
				securityCheck(); 
				keybytes.reveal();
				int result = keybytes.getLength(); 
				keybytes.hide();
				return result;
			}

			int getBitLength() {
				securityCheck();
				keybits.reveal();
				int result = keybits.getLength(); 
				keybits.hide();
				return result;
			}

			data::Bytes getBytes() {
				securityCheck();
				keybytes.reveal();
				data::Bytes result(keybytes.getLength());
				keybytes.copyTo(result);
				keybytes.hide();
				return result;
			}

			data::Bits getBits() {
				securityCheck();
				keybits.reveal();
				data::Bits result(keybits.getLength());
				keybits.copyTo(result);
				keybits.hide();
				return result;
			}

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

			static Key random(int length, data::DataRepr dr=data::bits) {
				if (dr == data::bits) {
					return Key(csprng::bytes(length / 8));
				} else {
					return Key(csprng::bytes(length));
				}
			}

			static Key generate();

	};

};