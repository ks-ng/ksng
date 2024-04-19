#include "../core/cipher.h"
#include "../core/key.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cmath>

namespace aes {

    // class AdvancedEncryptionStandard: public cipher::SymmetricCipher {

	// 	private:

	// 		AES_KEY makeAESKey(key::Key k) {
	// 			AES_KEY result;
	// 			unsigned char buf[32];
	// 			for (int i = 0; i < 32; i++) {
	// 				buf[i] = k.getByte(i);
	// 			}
	// 			AES_set_encrypt_key(buf, 256, &result);
	// 			return result;
	// 		}

	// 	public:

	// 		data::Bytes encryptBlock(data::Bytes plaintext, key::Key k) {
	// 			AES_KEY aesk = makeAESKey();
	// 			unsigned char pt[16];
	// 			for (int i = 0; i < 16; i++) {
	// 				pt[i] = plaintext.get(i);
	// 			}
	// 			unsigned char ct[16];
	// 			AES_encrypt(pt, ct, &aesk);
	// 			data::Bytes result(16);
	// 			for (int i = 0; i < 16; i++) {
	// 				result.set(i, ct[i]);
	// 			}
	// 			return result;
	// 		}

	// 		data::Bytes decryptBlock(data::Bytes plaintext, key::Key k) {
	// 			AES_KEY aesk = makeAESKey();
	// 			unsigned char pt[16];
	// 			for (int i = 0; i < 16; i++) {
	// 				pt[i] = plaintext.get(i);
	// 			}
	// 			unsigned char ct[16];
	// 			AES_decrypt(pt, ct, &aesk);
	// 			data::Bytes result(16);
	// 			for (int i = 0; i < 16; i++) {
	// 				result.set(i, ct[i]);
	// 			}
	// 			return result;
	// 		}

	// 		key::Key generateKey() override {
	// 			return key::Key(csprng::bytes(256));
	// 		}

	// };

	// // all deprecated with OpenSSL 3.0

	class AES_CBC: public cipher::SymmetricCipher {

		private:

			void encryptBuffer(const unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
				EVP_CIPHER_CTX *ctx;

				// Create and initialize the context
				ctx = EVP_CIPHER_CTX_new();
				EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

				// Encrypt the data
				int len;
				EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);

				// Finalize the encryption
				int ciphertext_len = len;
				EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
				ciphertext_len += len;

				// Clean up
				EVP_CIPHER_CTX_free(ctx);
			}

			void decryptBuffer(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
				EVP_CIPHER_CTX *ctx;

				// Create and initialize the context
				ctx = EVP_CIPHER_CTX_new();
				EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

				// Decrypt the data
				int len;
				EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);

				// Finalize the decryption
				int plaintext_len = len;
				EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
				plaintext_len += len;

				// Clean up
				EVP_CIPHER_CTX_free(ctx);
			}

		public:

			data::Bytes encrypt(data::Bytes plaintext, key::Key k) override {

				// Prep text data
				int clen = (int)(16 * ceil(plaintext.getLength() / 16));
				unsigned char plaintextBuffer[plaintext.getLength()];
				for (int i = 0; i < plaintext.getLength(); i++) {
					plaintextBuffer[i] = plaintext.get(i);
				}
				unsigned char ciphertextBuffer[clen];

				// Prep key data
				unsigned char keyBuffer[32];
				data::Bytes keyData = k.getBytes().subbytes(0,32);
				for (int i = 0; i < keyData.getLength(); i++) {
					keyBuffer[i] = keyData.get(i);
				}
				unsigned char ivBuffer[16];
				data::Bytes iv = k.getBytes().subbytes(32,48);
				for (int i = 0; i < iv.getLength(); i++) {
					ivBuffer[i] = iv.get(i);
				}

				// All done
				encryptBuffer(plaintextBuffer, plaintext.getLength(), keyBuffer, ivBuffer, ciphertextBuffer);
				return data::Bytes(ciphertextBuffer, clen);
			}

			data::Bytes decrypt(data::Bytes ciphertext, key::Key k) override {
				
				// Prep text data
				int l = ciphertext.getLength();
				unsigned char plaintextBuffer[l];
				unsigned char ciphertextBuffer[l];
				for (int i = 0; i < ciphertext.getLength(); i++) {
					ciphertextBuffer[i] = ciphertext.get(i);
				}

				// Prep key data
				unsigned char keyBuffer[32];
				data::Bytes keyData = k.getBytes().subbytes(0,32);
				for (int i = 0; i < keyData.getLength(); i++) {
					keyBuffer[i] = keyData.get(i);
				}
				unsigned char ivBuffer[16];
				data::Bytes iv = k.getBytes().subbytes(32,48);
				for (int i = 0; i < iv.getLength(); i++) {
					ivBuffer[i] = iv.get(i);
				}

				// All done
				decryptBuffer(ciphertextBuffer, ciphertext.getLength(), keyBuffer, ivBuffer, plaintextBuffer);
				return data::Bytes(plaintextBuffer, l);
			}

			key::Key generateKey() override {
				return key::Key(csprng::bytes(48));
			}

	};

};