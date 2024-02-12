#include "../crypto/core/cipher.h"
#include "../crypto/core/hash.h"
#include "../crypto/core/suite.h"

namespace cryptanalyst {

	// Cryptanalyst class - intended to be overridden

	class Cryptanalyst {

		private:

			bool hasPlaintext = false;
			bool hasCiphertext = false;
			bool hasPlaintexts = false;
			bool hasCiphertexts = false;

		protected:

			data::Bytes plaintext;
			data::Bytes ciphertext;
			sda::SDA<data::Bytes> plaintexts;
			sda::SDA<data::Bytes> ciphertexts;
			suite::CryptographicSuite cryptosuite;

			virtual data::Bytes _cryptanalyze() = 0;

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

			void setPlaintext(data::Bytes pt) { hasPlaintext = true; plaintext = pt; }
			void setCiphertext(data::Bytes ct) { hasCiphertext = true; ciphertext = ct; }
			void setPlaintexts(sda::SDA<data::Bytes> pts) { hasPlaintexts = true; plaintexts = pts; }
			void setCiphertexts(sda::SDA<data::Bytes> cts) { hasCiphertexts = true; ciphertexts = cts; }

			virtual bool needsPlaintext() = 0;
			virtual bool needsCiphertext() = 0;
			virtual bool needsPlaintexts() = 0;
			virtual bool needsCiphertexts() = 0;

			virtual data::Bytes cryptanalyze() {
				if (
					(needsPlaintext() && !hasPlaintext)
					|| (needsCiphertext() && !hasCiphertext)
					|| (needsPlaintexts() && !hasPlaintexts)
					|| (needsCiphertexts() && !hasCiphertexts)
				) {
					notif::error("insufficient data to cryptanalyze");
					return data::Bytes(0);
				}

				return _cryptanalyze();
			}

	};

};