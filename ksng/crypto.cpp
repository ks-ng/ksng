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
				
				Bytestring ciphertext = nullString(plaintext.length);
				Bytestring block = nullString(blockSize);

				int blockIndex = 0;
				for (int i = 0; i < plaintext.length; i++) {
					if ((i == 0) || (i % blockSize != 0)) {
						// Block is incomplete
						block.data[i % blockSize] = plaintext.data[i];
					} else {
						// Block completed, encrypt it and add it 
						// to the ciphertext
						block = encryptBlock(block, key);
						block.copyTo(ciphertext, blockSize * blockIndex);
						blockIndex++;
					}
				}
				
				return ciphertext;
			}
		}

		Bytestring decrypt(Bytestring ciphertext, Cipher::Key key, Bytestring iv, Mode mode=ECB) {
			if (mode == ECB) {
				if (ciphertext.length % blockSize != 0) {
					return nullString(1);
				}
				
				Bytestring plaintext = nullString(ciphertext.length);
				Bytestring block = nullString(blockSize);

				int blockIndex = 0;
				for (int i = 0; i < ciphertext.length; i++) {
					if ((i == 0) || (i % blockSize != 0)) {
						// Block is incomplete
						block.data[i % blockSize] = ciphertext.data[i];
					} else {
						// Block completed, encrypt it and add it 
						// to the ciphertext
						block = decryptBlock(block, key);
						block.copyTo(plaintext, blockSize * blockIndex);
						blockIndex++;
					}
				}
				
				return plaintext;
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