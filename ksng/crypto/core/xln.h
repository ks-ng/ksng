#pragma once
#include "../../util/csprng.h"

namespace xln {

	class ExtraLargeNumber: public data::Bits {

		public:

			using data::Bits::Bits;

			// Comparison

			bool operator<(ExtraLargeNumber other) {
				for (int i = getLength() - 1; i >= 0; i--) {
					if (get(i) > other.get(i)) {
						return false;
					}
				}
				return true;
			}

			// Math

			ExtraLargeNumber operator^(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < getLength(); i++) { result.set(i, get(i) ^ other.get(i)); }
				return result;
			}

			ExtraLargeNumber operator>>(int bits) {
				ExtraLargeNumber result(getLength() + bits);

				for (int i = 0; i < getLength(); i++) { result.set(i + bits, get(i)); }

				return result;
			}

			ExtraLargeNumber operator<<(int bits) {
				ExtraLargeNumber result(getLength() - bits);
				for (int i = 0; i < getLength() - bits; i++) { result.set(i, get(i)); }
				return result;
			}

			ExtraLargeNumber operator+(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());

				int carry = 0;
				int a;
				int b;
				for (int i = 0; i < getLength(); i++) {
					if (i >= other.getLength()) { break; }
					a = get(i);
					b = other.get(i);
					result.set(i, ((a ^ b) ^ carry));
					carry = (a & b) | (a & carry) | (b & carry); 
				}

				return result;
			}

			ExtraLargeNumber operator++(int x) {
				ExtraLargeNumber one(getLength());
				one.set(0, 1);
				return *this + one;
			}

			ExtraLargeNumber operator*(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength() + other.getLength());
				for (int i = 0; i < other.getLength(); i++) { 
					if (other.get(i) == 1) { 
						result = (result + ((*this) >> i) << (result.getLength() - getLength()); 
					} 
				}
				return result;
			}

			ExtraLargeNumber exponentiate(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < other.getLength(); i++) { 
					if (other.get(i) == 1) { 
						result = (result * ((*this) >> i)) << (result.getLength() - getLength()); 
					}
				}
				return result;
			}


	};

	using XLN = ExtraLargeNumber;

};
