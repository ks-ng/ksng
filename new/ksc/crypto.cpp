#pragma once

class Bytestring {

	public:

		Bytestring(unsigned char _data[], const int _length) {
			const int length = _length;
			unsigned char data[length];
			for (int i = 0; i < length; i++) {
				data[i] = _data[i];
			}
		}

		void copyTo(Bytestring other, int offset=0) {
			for (int i = 0; i < length; i++) {
				other.data[i + offset] = data[i];
			}
		}

		Bytestring xor(Bytestring other) {
			unsigned char xored[length];
			for (int i = 0; i < length; i++) {
				xored[i] = data[i] ^ other.data[i];
			}
			return Bytestring(xored, length);
		}

}

Bytestring nullString(const int length) {
	unsigned char nullBytestring[length];
	return Bytestring(nullBytestring, length);
}

// Encryption - securing data

class Cipher {

	public:

		class Key {

			private:

				Bytestring keyData = nullString(256);
				bool hidden = false;

			public:

				Key(Bytestring data) {
					const int length = data.length;
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

		virtual int blockSize;
		Bytestring nullBlock = nullString(blockSize);

		char ECB = "E";
		char CBC = "C";
		char CFB = "F";

		virtual Bytestring encryptBlock(Bytestring plaintext[], Cipher::Key key) = 0;
		virtual Bytestring decryptBlock(Bytestring ciphertext[], Cipher::Key key) = 0;

		Bytestring encrypt(Bytestring plaintext[], Cipher::Key key, Bytestring iv=nullBlock, Mode mode=ECB) {
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
						block.data[i % blockSize] = plaintext[i];
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

		Bytestring decrypt(Bytestring ciphertext[], Cipher::Key key, Bytestring iv=nullBlock, Mode mode=ECB) {
			if (mode == ECB) {
				if (plaintext.length % blockSize != 0) {
					return nullString(1);
				}
				
				Bytestring plaintext = nullString(plaintext.length);
				Bytestring block = nullString(blockSize);

				int blockIndex = 0;
				for (int i = 0; i < ciphertext.length; i++) {
					if ((i == 0) || (i % blockSize != 0)) {
						// Block is incomplete
						block.data[i % blockSize] = ciphertext[i];
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

		virtual Bytestring keystream(Key key, const int length) = 0;

		Bytestring encrypt(Bytestring plaintext, Cipher::Key key) {
			return plaintext.xor(keystream(key, plaintext.length));
		}

		Bytestring decrypt(Bytestring ciphertext, Cipher::Key key) {
			return ciphertext.xor(keystream(key, ciphertext.length));
		}
};

// Hashing - verifying data
class Hash {

	public:

		enum saltMode { prefix, suffix, add };

		virtual unsigned char* hash(unsigned char text[], const int length) = 0;

		unsigned char* saltedHash(
				unsigned char text[], 
				const int textLength,
				unsigned char salt[], 
				const int saltLength,
				saltMode mode ) {

			if (mode == add) {
				const int combinedLength = textLength;
				unsigned char combined[textlength];
				for (int i = 0; i < textLength; i++) {
					combined[i] = text[i] ^ salt[i];
				}
			} else if (mode == prefix) {
				const int combinedLength = textLength + saltLength;
				unsigned char combined[combinedLength];
				int i = 0;
				for (int j = 0; j < saltLength; j++) {
					combined[i] = salt[j];
					i++;
				}
				for (int j = 0; j < textLength; j++) {
					combined[i] = salt[j];
					i++;
				}
			} else if (mode == suffix) {
				const int combinedLength = textLength + saltLength;
				unsigned char combined[combinedLength];
				int i = 0;
				for (int j = 0; j < textLength; j++) {
					combined[i] = text[j];
					i++;
				}
				for (int j = 0; j < saltLength; j++) {
					combined[i] = salt[j];
					i++;
				}
			}
			
			return hash(combined, combinedLength);

		}
};
