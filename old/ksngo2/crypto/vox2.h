#include "cryptocore.h"

using namespace std;

class VOX2256: cryptocore::Cipher {

	// VOX2, running 2048-bit (256-byte) keys.

	public:

		datacls::Data encrypt(datacls::Data plaintext, cryptocore::Key key) override {

			if (key.length() != 256) {
				notif::error("VOX2: key length must be 256 bytes");
				return plaintext;
			}

			if (plaintext.length() != 128) {
				notif::error("VOX2: plaintext length must be 128 bytes");
				return plaintext;
			}

			datacls::Data y = csprng(128);
			datacls::Data x = plaintext + y;
			datacls::Data z(256);
			x.copyInto(z, 0);
			y.copyInto(z, 256);

			// Permute the result (otherwise no encryption takes place)

			datacls::Data result(256);
			for (int i = 0; i < 256; i++) {
				result.setByte(key.getByte(i), z.getByte(i));
			}

			return result;

		}

		datacls::Data decrypt(datacls::Data ciphertext, cryptocore::Key key) override {
			// reverse the permutation first

			if (key.length() != 256) {
				notif::error("VOX2: key length must be 256 bits");
				return ciphertext;
			}

			if (plaintext.length() != 256) {
				notif::error("VOX2: ciphertext length must be 256 bits");
				return ciphertext;
			}

			datacls::Data z(256);


		}

}