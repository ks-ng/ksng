#pragma once
#include "cipher.h"
#include "hash.h"

namespace suite {

	class CryptographicSuite {

		private:
		
			cipher::SymmetricCipher* sc;
			cipher::AsymmetricCipher* asc;
			hash::HashFunction* hf;
			hash::KeyedHashFunction khf;

		public:

			CryptographicSuite(
				cipher::SymmetricCipher* sc = nullptr,
				cipher::AsymmetricCipher* asc = nullptr,
				hash::HashFunction* hf = nullptr,
				hash::KeyedHashFunction* khf = nullptr
			): sc(sc), asc(asc), hf(hf), khf(khf) {}

			// Post-initialization setup

			void setSymmetricCipher(cipher::SymmetricCipher* sc_) { sc = sc_; }
			void setAsymmetricCipher(cipher::AsymmetricCipher* asc_) { asc = asc_; }
			void setHashFunction(hash::HashFunction* hf_) { hf = hf_; }
			void setKeyedHashFunction(hash::KeyedHashFunction* khf_) { khf = khf_; }

			// Cryptographic methods

			data::Bytes encryptSymmetric(data::Bytes plaintext, key::Key k) { return sc->encrypt(plaintext, k); }
			data::Bytes encryptAsymmetric(data::Bytes plaintext, key::Key k) { return asc->encrypt(plaintext, k); }
			data::Bytes decryptSymmetric(data::Bytes ciphertext, key::Key k) { return sc->decrypt(ciphertext, k); }
			data::Bytes decryptAsymmetric(data::Bytes ciphertext, key::Key k) { return asc->decrypt(ciphertext, k); }
			data::Bytes encryptFileSymmetric(string filename, key::Key k) { return sc->encryptFile(filename, k); }
			data::Bytes encryptFileAsymmetric(string filename, key::Key k) { return asc->encryptFile(filename, k); }
			data::Bytes decryptFileSymmetric(string filename, key::Key k) { return sc->decryptFile(filename, k); }
			data::Bytes decryptFileAsymmetric(string filename, key::Key k) { return asc->decryptFile(filename, k); }
			data::Bytes hash(data::Bytes plaintext) { return hf->hash(plaintext); }
			data::Bytes hashFile(string filename, string store=(string)("NONE")) { return hf->hashFile(filename, store); }
			data::Bytes keyedHash(data::Bytes plaintext, key::Key k) { return khf->hash(plaintext, k); }
			data::Bytes keyedHashFile(string filename, string store=(string)("NONE")) { return khf->hashFile(filename, store); }

	};

	using Suite = CryptographicSuite;
	using CS = CryptographicSuite;

};