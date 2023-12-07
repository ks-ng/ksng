#include "../utils.cpp" // for bytestring
#include "../err.cpp" // for custom errors

class Key final {
	
	private:

		Bytestring keydata;
		bool accessible = false;

	public:

		Key(Bytestring data) {
			keydata = data;
			data.erase();
		}

		Key(Bytestring* data) {
			keydata = *data;
		}

		Key(string filename) {
			keydata = readFile(filename);
		}

		~Key() {
			keydata.erase();
			hide();
		}

		inline void hide() { accessible = false; }
		inline void reveal() { accessible = true; }
		inline void setStatus(bool value) { accessible = value; }

		unsigned char operator[](int index) {
			if (!accessible) {
				panic((string)("Access denied to this key."));
			} else {
				return keydata[index];
			}
		}

		Bytestring access() {
			if (!accessible) {
				panic((string)("Access denied to this key."));
				return Bytestring(0);
			} else {
				return keydata;
			}
		}

};

class Cipher {

	public:

		Cipher() {}

		virtual Bytestring encrypt(Bytestring data, Key key) = 0;
		virtual Bytestring decrypt(Bytestring data, Key key) = 0;

		void encryptFile(std::string filename, Key key) {
			Bytestring plaintext = readFile(filename);
			Bytestring ciphertext = encrypt(plaintext, key);
			plaintext.erase();
			writeFile(filename, ciphertext);
			ciphertext.erase();
		}

		void decryptFile(std::string filename, Key key) {
			Bytestring ciphertext = readFile(filename);
			Bytestring plaintext = decrypt(ciphertext, key);
			ciphertext.erase();
			writeFile(filename, plaintext);
			plaintext.erase();
		}

};