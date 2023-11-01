#pragma once

// Encryption - securing data

class Encryption {

	public:

		template <const int len>
		class Key {

			private:

				unsigned char keyData[len];

			public:

				const int length = len;

				Key(unsigned char data[len]) {
					for (int i = 0; i < length; i++) {
						keyData[i] = data[i];
					}
				}

		}

		virtual unsigned char* encrypt(unsigned char plaintext[], Key key, const int length) = 0;
		virtual unsigned char* decrypt(unsigned char ciphertext[], Key key, const int length) = 0;

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
