#pragma once
#include <cmath>
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
				if (a == 0 && b == 0) {
					result.set(i, carry);
					carry = 0;
				} else if (carry == 0 && ((a == 1 && b == 0) || (a == 0 && b == 1))) {
					result.set(i, 1);
				} else if (carry == 1 && ((a == 1 && b == 0) || (a == 0 && b == 1))) {
					result.set(i, 0);
					carry = 1;
				} else {
					result.set(i, 1);
					carry = 1;
				}
			}

			return result;
		}

		Cryptonum operator*(Cryptonum other) {

		}

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