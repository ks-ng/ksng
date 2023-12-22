#pragma once

#include "../util/fileops.h"

using namespace std;

namespace cryptocore {

	datacls::Data csprng(int length) {
		return fileops::readBytes("/dev/urandom", length);
	}

	class Key {

		private:

			datacls::Data fragments[2];
			bool locked = false;
			int length;

		public:

			Key(string filename, int fragmentCount=5) {
				fragments[0] = fileops::readFile(filename);
				length = fragments[0].length(bytes);
				fragments[1] = csprng(fragments[0].length(datacls::bytes));
				fragments[0] = fragments[0] + fragments[1];
			}

			~Key() {
				fragments[0].erase();
				fragments[1].erase();
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void accessSecurityCheck() {
				if (locked) {
					notif::fatal("attempted access to locked key, security warning");
				}
			}

			// Methods

			int length() {
				accessSecurityCheck();
				return length;
			}

			unsigned char getByte(int byteOffset) { 
				accessSecurityCheck(); 
				return fragments[0].getByte(byteOffset) ^ fragments[1].getByte(byteOffset); 
			}

	};

	class Cipher {

		public:

			virtual datacls::Data encrypt(datacls::Data plaintext, Key key) = 0;
			virtual datacls::Data decrypt(datacls::Data ciphertext, Key key) = 0;

			void encryptFile(string filename, Key key) {
				datacls::Data plaintext = fileops::readFile(filename);
				fileops::writeFile(filename, encrypt(plaintext, key));
			}

			void decryptFile(string filename, Key key) {
				datacls::Data ciphertext = fileops::readFile(filename);
				fileops::writeFile(filename, decrypt(ciphertext, key));
			}

	};

};