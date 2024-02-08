#pragma once
#include "key.h"

namespace cipher {

	enum SymmetricCipherMode {
		ECB = 1,
		CBC = 2,
		CFB = 3,
		OFB = 4,
		CTR = 5
	};

	struct Keypair {
		key::Key publicKey;
		key::Key privateKey;
	};

	class Cipher {

		public:

			virtual data::Bytes encrypt(data::Bytes plaintext, key::Key key) = 0;
			virtual data::Bytes decrypt(data::Bytes ciphertext, key::Key key) = 0;

			void encryptFile(string filename, key::Key key) {
				data::Bytes plaintext = fileops::readFileBytes(filename);
				fileops::writeFile(filename, encrypt(plaintext, key));
			}

			void decryptFile(string filename, key::Key key) {
				data::Bytes ciphertext = fileops::readFileBytes(filename);
				fileops::writeFile(filename, decrypt(ciphertext, key));
			}

	};

	class SymmetricCipher: public Cipher {

		public:

			virtual key::Key generateKey() = 0;

	};

	class AsymmetricCipher: Cipher {

		public:

			virtual Keypair generateKeys() = 0;

	};

};