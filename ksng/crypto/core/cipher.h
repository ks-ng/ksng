#pragma once
#include "key.h"

namespace cipher {

	class Cipher {

		public:

			virtual data::Bytes encrypt(data::Bytes, key::Key) = 0;
			virtual data::Bytes decrypt(data::Bytes, key::Key) = 0;

			void encryptFile(string filename, key::Key key) {
				data::Bytes plaintext = fileops::readFileBytes(filename);
				fileops::writeFile(filename, encrypt(plaintext, key));
			}

			void decryptFile(string filename, key::Key key) {
				data::Bytes ciphertext = fileops::readFileBytes(filename);
				fileops::writeFile(filename, decrypt(ciphertext, key));
			}

	};

};