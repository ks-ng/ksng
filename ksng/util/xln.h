#pragma once
#include "csprng.h"

namespace xln {

	class ExtraLargeNumber: public data::Bits {

		public:

			using data::Bits::Bits;

			// Comparison

			bool isZero() { for (int i = 0; i < getLength(); i++) { if (get(i) != 0) { return false; } } return true; }

			bool operator<(ExtraLargeNumber other) {
				if (getLength() > other.getLength()) { return false; }
				for (int i = getLength() - 1; i >= 0; i--) {
					if (get(i) > other.get(i)) {
						return false;
					}
				}
				return true;
			}

			bool operator==(ExtraLargeNumber other) {
				for (int i = getLength() - 1; i >= 0; i--) {
					if (get(i) != other.get(i)) {
						return false;
					}
				}
				return true;
			}

			bool operator>(ExtraLargeNumber other) {
				if (getLength() < other.getLength()) { return false; }
				for (int i = getLength() - 1; i >= 0; i--) {
					if (i > other.getLength()) { return true; }
					if (get(i) < other.get(i)) {
						return false;
					}
				}
				return true;
			}

			// Logical operators

			ExtraLargeNumber operator^(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				int i;
				int j;
				for (i = 0; i < getLength() && i < other.getLength(); i++) { 
					result.set(i, get(i) ^ other.get(i)); 
				}
				for (j = i; j < other.getLength(); j++) {
					result.set(j, get(j));
				}
				return result;
			}

			ExtraLargeNumber operator&(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				int i;
				int j;
				for (i = 0; i < getLength() && i < other.getLength(); i++) { 
					result.set(i, get(i) & other.get(i)); 
				}
				for (j = i; j < other.getLength(); j++) {
					result.set(j, get(j));
				}
				return result;
			}

			ExtraLargeNumber operator|(ExtraLargeNumber other) {
				ExtraLargeNumber result(getLength());
				int i;
				int j;
				for (i = 0; i < getLength() && i < other.getLength(); i++) { 
					result.set(i, get(i) | other.get(i)); 
				}
				for (j = i; j < other.getLength(); j++) {
					result.set(j, get(j));
				}
				return result;
			}

			ExtraLargeNumber operator~() {
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < getLength(); i++) { result.set(i, get(i) ^ 1); }
				return result;
			}

			// Arithmetic

			// * 2
			ExtraLargeNumber operator>>(int bits) {
				ExtraLargeNumber result(getLength() + bits);

				for (int i = 0; i < getLength(); i++) { result.set(i + bits, get(i)); }

				return result;
			}

			// / 2
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
				for (int i = 0; i < getLength() && i >= other.getLength(); i++) {
					a = get(i);
					b = other.get(i);
					result.set(i, ((a ^ b) ^ carry));
					carry = (a & b) | (a & carry) | (b & carry); 
				}

				return result;
			}

			ExtraLargeNumber operator-(ExtraLargeNumber other) {
				// calculate a - b
				ExtraLargeNumber a = copy();
				ExtraLargeNumber b = other.copy();
				ExtraLargeNumber result(getLength());
				int borrow = 0;
				int bitA;
				int bitB;
				for (int i = 0; i < getLength() && i < other.getLength(); i++) {
					bitA = get(i);
					bitB = other.get(i);
					if (bitB > bitA) { borrow++; }
					result.set(i, )
				}
				return result;
			}

			ExtraLargeNumber operator*(ExtraLargeNumber other) {
				ExtraLargeNumber this_ = copy();
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < other.getLength(); i++) { 
					if (other.get(i) == 1) { 
						result = result + (copy() >> i);
					} 
				}
				return result;
			}

			ExtraLargeNumber exponentiate(ExtraLargeNumber other) {
				ExtraLargeNumber this_ = copy();
				ExtraLargeNumber result(getLength());

				sda::SDA<ExtraLargeNumber> powersOfTwo(other.getLength()); // probably don't need this much space but whatever
				powersOfTwo.set(0, this_);
				for (int i = 1; i < other.getLength(); i++) {
					powersOfTwo.set(i, powersOfTwo.get(i - 1) * powersOfTwo.get(i - 1));
				}

				result.set(0, 1);
				for (int i = 0; i < other.getLength(); i++) {
					if (other.get(i) == 1) {
						result = (result * powersOfTwo.get(i));
					}
				}
				return result;
			}

			// utility

			ExtraLargeNumber copy() {
				ExtraLargeNumber result(getLength());
				for (int i = 0; i < getLength(); i++) { result.set(i, get(i)); }
				return result;
			}

			string num() {
				stringstream ss;
				for (int i = getLength() - 1; i >= 0; i--) {
					ss << get(i);
					if (i % 8 == 0 && i != 0) {
						ss << "-";
					}
				}
				return ss.str();
			}

	};

	using XLN = ExtraLargeNumber;

	XLN random(int bitlength) {
		data::Bits d = csprng::bits(bitlength);
		d.set(bitlength - 1, 1);
		XLN r(bitlength);
		for (int i = 0; i < bitlength; i++) { r.set(i, d.get(i)); }
		return r;
	}

	inline XLN randomEven(int bitlength) {
		XLN r = random(bitlength);
		r.set(0, 0);
		return r;
	}

	inline XLN randomOdd(int bitlength) {
		XLN r = random(bitlength);
		r.set(0, 1);
		return r;
	}

};
