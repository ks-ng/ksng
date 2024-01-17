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
} // effective key size is around 1700 bits

class VOX: public cipher::Cipher {

	public:

		

};