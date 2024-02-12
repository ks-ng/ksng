#include "../crypto/core/cipher.h"
#include "../crypto/core/hash.h"
#include "../crypto/core/suite.h"

namespace cryptanalyst {

	// Cryptanalyst class - intended to be overridden

	class Cryptanalyst {

		private:

			data::Bytes plaintext;
			data::Bytes ciphertext;
			sda::SDA<data::Bytes> plaintexts;
			sda::SDA<data::Bytes> ciphertexts;
			suite::CryptographicSuite cryptosuite;

		public:

			// setup

			Cryptanalyst(
				cipher::SymmetricCipher* sc = nullptr,
				cipher::AsymmetricCipher* asc = nullptr,
				hash::HashFunction* hf = nullptr,
				hash::KeyedHashFunction* khf = nullptr
			) {
				cryptosuite.setSymmetricCipher(sc);
				cryptosuite.setAsymmetricCipher(asc);
				cryptosuite.setHashFunction(hf);
				cryptosuite.setKeyedHashFunction(khf);
			}

			void setSymmetricCipher(cipher::SymmetricCipher* cipher) { cryptosuite.setSymmetricCipher(cipher); }
			void setAsymmetricCipher(cipher::AsymmetricCipher* cipher) { cryptosuite.setAsymmetricCipher(cipher); }
			void setHashFunction(hash::HashFunction* hashf) { cryptosuite.setHashFunction(hashf); }
			void setKeyedHashFunction(hash::KeyedHashFunction* hashf) { cryptosuite.setKeyedHashFunction(hashf); }

			void setPlaintext(data::Bytes pt) { plaintext = pt; }
			void setCiphertext(data::Bytes ct) { ciphertext = ct; }
			void setPlaintexts(sda::SDA<data::Bytes> pts) { plaintexts = pts; }
			void setCiphertexts(sda::SDA<data::Bytes> pts) { plaintexts = pts; }

			

	};

};