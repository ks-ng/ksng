#pragma once

#include "utils.cpp"

// Encryption - securing data

class Cipher {

	public:

		class Key {

			private:

				Bytestring keyData = nullString(256);
				bool hidden = false;

			public:

				int length;

				Key(Bytestring data) {
					length = data.length;
					for (int i = 0; i < length; i++) {
						keyData.data[i] = data.data[i];
					}
				}

				Bytestring getKeyData() {
					if (!hidden) {
						return keyData;
					} else {
						return nullString(length);
					}
				}

				void hide() {
					hidden = true;
				}

				void reveal() {
					hidden = false;
				}

		};

		virtual Bytestring encrypt(Bytestring plaintext, Cipher::Key key) = 0;
		virtual Bytestring decrypt(Bytestring ciphetext, Cipher::Key key) = 0;

};

class BlockCipher: Cipher {

	public:

		class Key: Cipher::Key {};

		int blockSize;

		enum Mode {ECB, CBC, CFB};

		virtual Bytestring encryptBlock(Bytestring plaintext, Cipher::Key key) = 0;
		virtual Bytestring decryptBlock(Bytestring ciphertext, Cipher::Key key) = 0;

		Bytestring encrypt(Bytestring plaintext, Cipher::Key key, Bytestring iv, Mode mode=ECB) {
			if (mode == ECB) {
				if (plaintext.length % blockSize != 0) {
					return nullString(1);
				}

				int blockCount = plaintext.length / blockSize;
				
				Bytestring ciphertext = nullString(plaintext.length);
				Bytestring block = nullString(blockSize);
				for (int i = 0; i < blockCount; i++) {
					block = plaintext.substring(blockSize * i, blockSize * (i + 1));
					ciphertext = ciphertext.concatenate(encryptBlock(block, key));
				}

				return ciphertext
			} else if (mode == CBC) {
				int blockCount = plaintext.length / blockSize;
				
				Bytestring ciphertext = nullString(plaintext.length);
				Bytestring block = nullString(blockSize);
				for (int i = 0; i < blockCount; i++) {
					block = plaintext.substring(blockSize * i, blockSize * (i + 1));
					encryptedBlock = encryptBlock(block.bitwiseXor(iv), key);
					iv = encryptedBlock;
					ciphertext = ciphertext.concatenate(encryptedBlock);
				}

				return ciphertext
			} else if (mode == CFB) {
			}
		}

		Bytestring decrypt(Bytestring ciphertext, Cipher::Key key, Bytestring iv, Mode mode=ECB) {
			if (mode == ECB) {
				if (ciphertext.length % blockSize != 0) {
					return nullString(1);
				}

				int blockCount = ciphertext.length / blockSize;
				
				Bytestring plaintext = nullString(ciphertext.length);
				Bytestring block = nullString(blockSize);
				for (int i = 0; i < blockCount; i++) {
					block = ciphertext.substring(blockSize * i, blockSize * (i + 1));
					ciphertext = ciphertext.concatenate(decryptBlock(block, key));
				}

				return plaintext
			} else if (mode == CBC) {
				int blockCount = ciphertext.length / blockSize;
				
				Bytestring plaintext = nullString(ciphertext.length);
				Bytestring block = nullString(blockSize);
				for (int i = 0; i < blockCount; i++) {
					block = ciphertext.substring(blockSize * i, blockSize * (i + 1));
					Bytestring decryptedBlock = decryptBlock(block, key);
					iv = decryptedBlock;
					plaintext = plaintext.concatenate(decryptedBlock.bitwiseXor(iv));
				}

				return ciphertext
			}
		}

};

class StreamCipher: Cipher {

	public:

		virtual Bytestring keystream(Cipher::Key key, const int length) = 0;

		Bytestring encrypt(Bytestring plaintext, Cipher::Key key) {
			return plaintext.bitwiseXor(keystream(key, plaintext.length));
		}

		Bytestring decrypt(Bytestring ciphertext, Cipher::Key key) {
			return ciphertext.bitwiseXor(keystream(key, ciphertext.length));
		}
};

// Hashing - verifying data
class Hash {

	public:

		enum mode {prefix, suffix, xoradd};

		virtual Bytestring hash(Bytestring text) = 0;

		Bytestring saltedHash(Bytestring text, Bytestring salt, mode m) {
			if (m == prefix) {
				Bytestring sum = salt.concatenate(text);
				return hash(sum);
			} else if (m == suffix) {
				Bytestring sum = text.concatenate(salt);
				return hash(sum);
			} else if (m == xoradd) {
				Bytestring sum = text.bitwiseXor(salt);
				return hash(sum);
			} else {}
		}

};

// Implementations

class AES: BlockCipher {
}
