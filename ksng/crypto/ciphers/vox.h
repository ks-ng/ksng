#pragma once
#include "../core/cipher.h"
#include "../core/fragment.h"
#include "../core/permute.h"
#include <cmath>

using namespace std;

namespace voxauth {

	data::Bytes VOX_AUTH_CALL("Virus in a lullaby, broken paradigms, all this deceiving; far above the sky, the end of all creation, once again; neverending...");
	data::Bytes VOX_AUTH_RESP("Disconnected, fighting for what you think is right, a million ways to save the world with all of them ending in eternal hellfire");

};

namespace vox {

	// Vector Operation Xnterchange (i know the name is silly)
	class VectorOperationInterchange: public cipher::SymmetricCipher {

		public:

			inline bool validateKey(key::Key key) { return (bool)(key.getBitLength() == 2048); }

			data::Bytes encryptBlock(data::Bytes plaintext, key::Key k) {
				if (plaintext.getLength() < 128) {
					data::Bytes n(128);
					plaintext.copyTo(n);
					for (int i = plaintext.getLength(); i < 128; i++) {
						n.set(i, csprng::bytes(1).get(0));
					}
					plaintext = n;
				} else if (plaintext.getLength() > 128) {
					notif::fatal("VOX can\'t encrypt blocks more than 128 bytes at a time at the moment");
				}
				data::Bytes result(256);
				sda::SDA<data::Bytes> fragments = fragment::split(plaintext);
				fragments.get(0).copyTo(result, 0);
				fragments.get(1).copyTo(result, 128);
				result = permute::permute(result, k.getBytes());
				return result;
			}

			data::Bytes decryptBlock(data::Bytes ciphertext, key::Key k) {
				if (ciphertext.getLength() != 256) {
					notif::fatal("invalid ciphertext block length");
				}
				data::Bytes result = permute::depermute(ciphertext, k.getBytes());
				sda::SDA<data::Bytes> fragments(2);
				fragments.set(0, result.subbytes(0, 128));
				fragments.set(1, result.subbytes(128, 256));
				return fragment::reassemble(fragments);
			}

			data::Bytes encrypt(data::Bytes plaintext, key::Key k) override {
				if (plaintext.getLength() != 128) { notif::warning("VOX padded plaintext; strip trailing bytes"); }
				if (plaintext.getLength() <= 128) {
					return encryptBlock(plaintext, k);
				} else {
					int blockCount = ceil((float)(plaintext.getLength()) / 128.0);
					data::Bytes ciphertext(blockCount * 256);
					for (int i = 0; i < blockCount; i++) {
						encryptBlock(plaintext.subbytes(i * 128, (i + 1) * 128), k).copyTo(ciphertext, i * 256);
					}
					return ciphertext;
				}
			};

			data::Bytes decrypt(data::Bytes ciphertext, key::Key k) override {
				// if (ciphertext.getLength() % 256 != 0) { notif::fatal("invalid ciphertext"); }
				int blockCount = ciphertext.getLength() / 256;
				data::Bytes plaintext(ciphertext.getLength() / 2);
				for (int i = 0; i < blockCount; i++) {
					decryptBlock(ciphertext.subbytes(i * 256, (i + 1) * 256), k).copyTo(plaintext, i * 128);
				}
				return plaintext;
			};

			key::Key generateKey() override {
				data::Bytes result(256);
				unsigned char r = csprng::bytes(1).get(0);
				for (int i = 0; i < 255; i++) {
					r = csprng::bytes(1).get(0);
					while (result.has(r)) {
						r += 1;
					}
					result.set(i, r);
				}
				return key::Key(result);
			}

	};

	using VOI = VectorOperationInterchange;
	using VOX = VectorOperationInterchange;
	
};