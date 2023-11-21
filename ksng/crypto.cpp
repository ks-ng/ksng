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

	Bytestring readBytes(const string& filename, int byteCount) {
		Bytestring result(byteCount);
		ifstream file(filename);

		char chara = 0;
		int i = 0;
		if (file.is_open()) {
			while (!file.eof()) {
				file.get(chara);
				result.data[i] = static_cast<unsigned char>(chara);
				i++;
			}
		}

		return result;
	}

	inline Bytestring readFile(const string& filename) {
		int fileSize = getFileSize(filename);
		return readBytes(filename, fileSize);
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

	class StreamCipher: public Cipher {

		public:

			virtual Bytestring keystream(Key key, int length) = 0;

			Bytestring encrypt(Bytestring plaintext, Key key) {
				key.reveal();
				Bytestring ks = keystream(key, plaintext.length);
				key.hide();
				return Bytestring(plaintext, ks);
			}

			Bytestring decrypt(Bytestring ciphertext, Key key) {
				key.reveal();
				Bytestring ks = keystream(key, ciphertext.length);
				key.hide();
				return Bytestring(ciphertext, ks);
			}

	};

	class BlockCipher: public Cipher {

		public:

			int blockSize;

			enum Mode {
				ECB,
				CBC,
			};

			Bytestring iv;
			Mode currentMode = ECB;

			BlockCipher(int blockSize_, Bytestring iv_=Bytestring(0), Mode mode=ECB) {
				iv = iv_;
				blockSize = blockSize_;
				setMode(mode);
			}

			void setMode(Mode mode) {
				currentMode = mode;
			}

			virtual Bytestring encryptBlock(Bytestring plaintext, Key key) = 0;
			virtual Bytestring decryptBlock(Bytestring ciphertext, Key key) = 0;

			int getBlockedLength(int rawLength) {
				int result = 0;
				while (result < rawLength) {
					result += blockSize;
				}
				return result;
			}

			Bytestring encrypt(Bytestring plaintext, Key key) {
				int blockedLength = getBlockedLength(plaintext.length);
				if (blockedLength != plaintext.length) {
					Bytestring padding(blockedLength - plaintext.length);
					plaintext = plaintext.concatenate(padding);
				}
				int blockCount = blockedLength / blockSize;

				Bytestring result(0);

				if (currentMode == ECB) {
					Bytestring plaintextBlock;
					Bytestring ciphertextBlock;
					key.reveal();
					for (int i = 0; i < blockCount; i++) {
						plaintextBlock = plaintext.substring(i * blockSize, ((i + 1) * blockSize) - 1);
						ciphertextBlock = encryptBlock(plaintextBlock, key);
						result = result.concatenate(ciphertextBlock);
					}
					key.hide();
				} else if (currentMode == CBC) {
					Bytestring mixin = iv;
					Bytestring plaintextBlock;
					Bytestring ciphertextBlock;
					key.reveal();
					for (int i = 0; i < blockCount; i++) {
						plaintextBlock = plaintext.substring(i * blockSize, ((i + 1) * blockSize) - 1);
						ciphertextBlock = encryptBlock(Bytestring(plaintextBlock, mixin), key);
						mixin = ciphertextBlock;
						result = result.concatenate(ciphertextBlock);
					}
					key.hide();
				}

				return result;
			}

			Bytestring decrypt(Bytestring ciphertext, Key key) {
				int blockedLength = getBlockedLength(ciphertext.length);
				if (blockedLength != ciphertext.length) {
					Bytestring padding(blockedLength - ciphertext.length);
					ciphertext = ciphertext.concatenate(padding);
				}
				int blockCount = blockedLength / blockSize;

				Bytestring result(0);

				if (currentMode == ECB) {
					Bytestring plaintextBlock;
					Bytestring ciphertextBlock;
					key.reveal();
					for (int i = 0; i < blockCount; i++) {
						ciphertextBlock = ciphertext.substring(i * blockSize, ((i + 1) * blockSize) - 1);
						plaintextBlock = decryptBlock(ciphertextBlock, key);
						result = result.concatenate(plaintextBlock);
					}
					key.hide();
				} else if (currentMode == CBC) {
					Bytestring mixin = iv;
					Bytestring plaintextBlock;
					Bytestring ciphertextBlock;
					key.reveal();
					for (int i = 0; i < blockCount; i++) {
						ciphertextBlock = ciphertext.substring(i * blockSize, ((i + 1) * blockSize) - 1);
						plaintextBlock = Bytestring(decryptBlock(ciphertextBlock, key), mixin);
						mixin = ciphertextBlock;
						result = result.concatenate(plaintextBlock);
					}
					key.hide();
				}

				return result;
			}

	};

};

namespace impl {

	class OneTimePad: public basic::Cipher {

		public:

			Bytestring encrypt(Bytestring plaintext, basic::Key key) {
				key.reveal();
				Bytestring result(plaintext, key.accessData());
				key.hide();
				return result;
			}

			Bytestring decrypt(Bytestring ciphertext, basic::Key key) {
				key.reveal();
				Bytestring result(ciphertext, key.accessData());
				key.hide();
				return result;
			}

	};

};