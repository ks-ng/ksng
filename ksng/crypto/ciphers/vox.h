#pragma once
#include "../core/cipher.h"
#include "../core/fragment.h"
#include "../core/permute.h"

key::Key key::Key::generate() { // generates 256-byte (2048-bit) keys for VOX
	data::Bytes result(256);
	unsigned char r = csprng::bytes(1).get(0);
	for (int i = 0; i < 255; i++) {
		r = csprng::bytes(1).get(0);
		while (result.has(r)) {
			r += 1;
		}
		result.set(i, r);
	}
	cout << result.hex() << endl;
	return key::Key(result);
} // effective key size is around 1600 bits - infeasible to brute-force

class VOX: public cipher::Cipher {

	public:

		inline bool validateKey(key::Key key) { return (bool)(key.getBitLength() == 2048); }

		data::Bytes encrypt(data::Bytes plaintext, key::Key) override {
			if (plaintext.getLength() < 128) {
				data::Bytes n(128);
				plaintext.copyTo(n);
				plaintext = n;
			} else if (plaintext.getLength() > 128) {
				notif::fatal("VOX can\'t encrypt more than 256 bytes at a time at the moment");
			}
			data::Bytes result(256);
			sda::SDA<data::Bytes> fragments = fragment::split(plaintext);
			fragments.get(0).copyTo(result, 0);
			fragments.get(1).copyTo(result, 128);
			return permute::permute(result, key.getBytes());
		}

};