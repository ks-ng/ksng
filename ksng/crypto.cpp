#include "utils.cpp"
#include <fstream>
#include <random>
#include <stdexcept>

bool DESTROY_KEYS_OUT_OF_SCOPE = false;

using namespace std;

class AccessDeniedError : public std::runtime_error {

	public:
	
		AccessDeniedError(const std::string& message) : std::runtime_error(message) {}

};

namespace fileops {

	void destroyData(void* ptr, size_t size) {
		for (int i = 0; i < 999; i++) {
			memset(ptr, 0, size);
			memset(ptr, 1, size);
			memset(ptr, rand() % 2, size);
		}
	}

	int getFileSize(const std::string& filename) {
		ifstream file(filename, ios::binary | ios::ate);
		if (!file.is_open()) {
			// Handle error: unable to open the file
			return -1;
		}

		int size = static_cast<int>(file.tellg());
		file.close();

		return size;
	}

	Bytestring readFile(const string& filename) {
		int fileSize = getFileSize(filename);
		Bytestring result(fileSize);
		ifstream file(filename);

		char chara = 0;
		int i = 0;
		if (file.is_open()) {
			while (!file.eof()) {
				file.get(chara);
				cout << chara;
				result.data[i] = static_cast<unsigned char>(chara);
				i++;
			}
		}

		return result;
	}

	int writeFile(string filename, Bytestring data) {
		ofstream file(filename, ios::binary | ios::trunc);
		if (!file.is_open()) {
			cerr << "fatal error: could not open file";
			return -1;
		}

		if (!file.good()) {
			cerr << "fatal error: could not write to file";
			return -2;
		}

		for (int i = 0; i < data.length; i++) {
			file << static_cast<char>(data[i]);
		}

		file.close();
		return 0;
	}

};

namespace basic {

	class Key {

		private:

			Bytestring* dataptr;
			int length;
			bool locked;

		public:

			Key(Bytestring* keyData) {
				dataptr = keyData;
				length = keyData->length;
				hide();
			}

			~Key() {
				if (DESTROY_KEYS_OUT_OF_SCOPE) {
					fileops::destroyData((void*)(dataptr->data), length);
				}
			}

			unsigned char operator[](int index) {
				if (locked) {
					throw AccessDeniedError("fatal error: cryptography error: access denied");
				}
				return (dataptr->data)[index];
			}

			void reveal() {
				locked = false;
			}
			
			void hide() {
				locked = true;
			}

			Bytestring accessData() {
				if (locked) {
					throw AccessDeniedError("fatal error: cryptography error: access denied");
				}
				return *dataptr;
			}

			int accessLength() {
				if (locked) {
					throw AccessDeniedError("fatal error: cryptography error: access denied");
				}
				return length;
			}

	};

	class Cipher {

		public:

			virtual Bytestring encrypt(Bytestring plaintext, Key key) = 0;
			virtual Bytestring decrypt(Bytestring ciphertext, Key key) = 0;

			int encryptFile(string filename, Key key) {
				cout << "hi";
				Bytestring plaintext = fileops::readFile(filename);
				key.reveal();
				Bytestring ciphertext = encrypt(plaintext, key);
				key.hide();
				return fileops::writeFile(filename, ciphertext);
			}

			int decryptFile(string filename, Key key) {
				Bytestring ciphertext = fileops::readFile(filename);
				key.reveal();
				Bytestring plaintext = decrypt(plaintext, key);
				key.hide();
				return fileops::writeFile(filename, plaintext);
			}

	};

};

namespace impl {

	class Increment: public basic::Cipher {

		public:

			Bytestring encrypt(Bytestring plaintext, basic::Key key) = 0;

	};

};