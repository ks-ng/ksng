// DELTA MINimization Electronically

#include "../core/analyst.h"
#include "../../util/csprng.h"

namespace deltamine {

	class EncryptionDeltamine: public cryptanalyst::Cryptanalyst {

		protected:

			bool needsPlaintext() override { return true; }
			bool needsCiphertext() override { return true; }
			bool needsPlaintexts() override { return false; }
			bool needsCiphertexts() override { return false; }

			data::Bytes _cryptanalyze() override {
				data::Bits test = csprng::bits(targetSize);
				data::Bytes transient;
				data::Bytes encryption;
				data::Bits encryptionBits;
				data::Bits ciphertextBits
				sda::SDA<int> deltas(targetSize);
				int delta;

				while (true) {
					
					// Search for the best modification to make and then make it

					for (int i = 0; i < targetSize; i++) {
						test.set(i, test.get(i) ^ 1);
						transient = data::bitsToBytes(test);

						encryption = cryptosuite.encryptSymmetric(plaintext, key::Key(transient));
						

						deltas.set(i, delta);
					}

				}
			}

	};

};