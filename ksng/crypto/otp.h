#pragma once
#include "core.cpp"
#include "err.cpp"

using namespace std;

class OneTimePad: public Cipher {

	public:

		Bytestring encrypt(Bytestring data, Key key) {
			key.reveal();
			Bytestring ct = data.bitwiseXor(key.access());
			key.hide();
			return ct;
		}

		Bytestring encrypt(Bytestring data, Key key) {
			key.reveal();
			Bytestring ct = data.bitwiseXor(key.access());
			key.hide();
			return ct;
		}

};