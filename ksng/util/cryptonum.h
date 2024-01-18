#pragma once
#include <cmath>
#include <numeric>
#include "data.h"
#include "csprng.h"

using namespace std;

class Cryptonum: data::Bits {

	public:

		using data::Bits::Bits;

		// Math stuff

		Cryptonum operator+(Cryptonum other) {
			// MODULAR
			Cryptonum result(getLength());

			int carry = 0;
			int a;
			int b;
			for (int i = 0; i < getLength(); i++) {
				a = get(i);
				b = other.get(i);
				result.set(i, ((a ^ b) ^ carry));
				carry = (a & b) | (a & carry)) | (b & carry); 
			}

			return result;
		}

		Cryptonum operator>>(int i) {
			Cryptonum result(getLength() - i);
			for (int j = 0; j < getLength() - i; j++) { result.set(j, get(j + i)); }
			return result;
		}

		Cryptonum operator<<(int i) {
			Cryptonum result(getLength() + i);
			for (int j = 0; j < i; j++) { result.set(j, 0); }
			for (int j = 0; j < getLength(); j++) { result.set(j + i, get(j)); }
			return result;
		}

		Cryptonum exponentiate(

		// Utility

		static Cryptonum random(int bitlength) {
			Cryptonum result(bitlength);
			result.set(bitlength - 1, 1);
			for (int i = 0; i < bitlength - 1; i++) {
				result.set(i, csprng::bits(1).get(0));
			}
			return result;
		}

};
