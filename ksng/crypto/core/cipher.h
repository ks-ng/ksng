#pragma once
#include "key.h"

namespace cipher {

	enum CipherMode {
		ECB = 1,
		CBC = 2,
		CFB = 3,
		OFB = 4,
		CTR = 5
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

			key::Key generateKey();

	};

	class StreamCipher: public Cipher {

		public:

			virtual data::Bytes keystream(key::Key key, int size) = 0;

			data::Bytes encrypt(data::Bytes plaintext, key::Key key) override { return plaintext ^ keystream(key, plaintext.getLength()); }
			data::Bytes decrypt(data::Bytes ciphertext, key::Key key) override { return ciphertext ^ keystream(key, ciphertext.getLength()); }

	};

	class BlockCipher: public Cipher {

		public:

			// Constructors

			CipherMode cipherMode;

			BlockCipher() {
				notif::warning("block cipher should be initialized with a mode of operation but wasn\'t");
			}

			BlockCipher(CipherMode mode): cipherMode(mode) {}

			void setCipherMode(CipherMode mode) { cipherMode = mode; }

			// Encryption and decryption methods

			virtual data::Bytes encryptBlock(data::Bytes plaintext, key::Key key) = 0;
			virtual data::Bytes decryptBlock(data::Bytes ciphertext, key::Key key) = 0;

			// data::Bytes encrypt(data::Bytes plaintext, key::Key key) override {
				
			// }

			// data::Bytes decrypt(data::Bytes ciphertext, key::Key key) override {

			// }

	};

};