#include "utils.cpp" // for bytestring
#include "err.cpp" // for custom errors

namespace basic {

	class Key final {
		
		private:

			Bytestring* keyptr;
			bool accessible = false;

		public:

			Key(Bytestring data) {
				keyptr = &data;
				data.erase();
			}

			Key(Bytestring* data) {
				keyptr = data;
			}

			~Key() {
				keyptr->erase();
				hide();
			}

			inline void hide() { accessible = false; }
			inline void reveal() { accessible = true; }
			inline void setStatus(bool value) { accessible = value; }

			Bytestring access() {
				if (!accessible) {
					throw AccessDeniedError();
				} else {
					return *keyptr;
				}
			}

	};

	class Cipher {

		private:

			Key* currentKey;

		public:

			Cipher() {}
			Cipher(Key key): currentKey(&key) {}
			Cipher(Key* key): currentKey(key) {}

			virtual Bytestring encrypt(Bytestring data, Key key) = 0;
			virtual Bytestring decrypt(Bytestring data, Key key) = 0;

			void encryptFile(std::string filename, Key key) {
				Bytestring plaintext = readFile(filename);
				Key.reveal();
				Bytestring ciphertext = encrypt(plaintext, key);
				Key.hide();
				plaintext.erase();
				writeFile(filename, ciphertext);
				ciphertext.erase();
			}

			void decryptFile(std::string filename, Key key) {
				Bytestring ciphertext = readFile(filename);
				Key.reveal();
				Bytestring plaintext = decrypt(ciphertext, key);
				Key.hide();
				ciphertext.erase();
				writeFile(filename, plaintext);
				plaintext.erase();
			}

	}

};