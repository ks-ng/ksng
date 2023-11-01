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

		void copyTo(Bytestring other) {
			for (int i = 0; i < length; i++) {
				other.data[i] = data[i];
			}
		}

		Bytestring null(const int length) {
			unsigned char nullBytestring[length];
			return Bytestring(nullBytestring, length);
		}
}

// Encryption - securing data

class Cipher {

	public:

		template <const int len>
		class Key {

			private:

				unsigned char _placeholder[len];
				Bytestring keyData = Bytestring(_placeholder, len);
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
						return Bytestring.null(length);
					}
				}

				void hide() {
					hidden = true;
				}

				void reveal() {
					hidden = false;
				}

		}

		virtual unsigned char* encrypt(unsigned char plaintext[], Key key, const int length) = 0;
		virtual unsigned char* decrypt(unsigned char ciphertext[], Key key, const int length) = 0;

};

class BlockCipher: Cipher {

	public:

		virtual unsigned char* encryptBlock(unsigned char plaintext[], Key key, const int length) = 0;
		virtual unsigned char* decryptBlock(unsigned char ciphertext[], Key key, const int length) = 0;

		unsigned char* encrypt(unsigned char plaintext[], 

};

// Hashing - verifying data
class Hash {

	public:

		enum saltMode { prefix, suffix, xor };

		virtual unsigned char* hash(unsigned char text[], const int length) = 0;

		unsigned char* saltedHash(
				unsigned char text[], 
				const int textLength,
				unsigned char salt[], 
				const int saltLength,
				saltMode mode ) {

			if (mode == xor) {
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
