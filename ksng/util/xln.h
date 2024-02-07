#pragma once
#include "csprng.h"

using namespace std;

namespace xln {

	class ExtraLargeNumber: public data::Bits {

		public:

			using data::Bits::Bits;

			// Comparison

			bool isZero() { for (int i = 0; i < getLength(); i++) { if (get(i) != 0) { return false; } } return true; }

			bool operator<(ExtraLargeNumber other) {
				for (int i = max(getLength(), other.getLength()); i >= 0; i--) {
					if (i < getLength() && i >= other.getLength()) {
						if (get(i) == 1) { return false; }
					} else if (i >= getLength() && i < other.getLength()) {
						if (other.get(i) == 1) { return true; }
					} else if (i < getLength() && i < other.getLength()) {
						if (get(i) ^ other.get(i) == 1) {
							if (get(i) == 1) { return false; }
							if (other.get(i) == 1) { return true; }
						}
					}
				}
				return false;
			}

			bool operator==(ExtraLargeNumber other) {
				for (int i = max(getLength(), other.getLength()); i >= 0; i--) {
					if (i < getLength() && i >= other.getLength()) {
						if (get(i) == 1) { return false; }
					} else if (i >= getLength() && i < other.getLength()) {
						if (other.get(i) == 1) { return false; }
					} else if (i < getLength() && i < other.getLength()) {
						if (get(i) ^ other.get(i) == 1) { return false; }
					}
				}
				return true;
			}

			bool operator>(ExtraLargeNumber other) {
				for (int i = max(getLength(), other.getLength()); i >= 0; i--) {
					if (i < getLength() && i >= other.getLength()) {
						if (get(i) == 1) { return true; }
					} else if (i >= getLength() && i < other.getLength()) {
						if (other.get(i) == 1) { return false; }
					} else if (i < getLength() && i < other.getLength()) {
						if (get(i) ^ other.get(i) == 1) {
							if (get(i) == 1) { return true; }
							if (other.get(i) == 1) { return false; }
						}
					}
				}
				return false;
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
				for (int i = 0; i < getLength(); i++) {
					if (i >= other.getLength()) { result.set(i, get(i)); continue; }
					if (i >= getLength()) { result.set(i, other.get(i)); continue; }
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
				ExtraLargeNumber b(other.getLength());
				for (int i = 0; i < other.getLength(); i++) { b.set(i, other.get(i) ^ 1); }
				ExtraLargeNumber one(8);
				one.set(0, 1);
				b = b;
				return a + b + one;
			}

			ExtraLargeNumber operator*(ExtraLargeNumber other) {
				ExtraLargeNumber this_ = copy();
				ExtraLargeNumber result(getLength() + other.getLength());
				for (int i = 0; i < other.getLength(); i++) { 
					if (other.get(i) == 1) { 
						result = result + (copy() >> i);
					} 
				}
				return result;
			}

			ExtraLargeNumber operator%(ExtraLargeNumber other) {
				ExtraLargeNumber a = copy();
				ExtraLargeNumber b = other.copy();
				if (a < b) {
					return a; // slight optimization for edge cases
				} else if (a == b) {
					return ExtraLargeNumber(a.getLength()); // another slight optimization
				} else {
					int i = 0;
					for (i = 0; (b >> i) < a; i++);
					i--;
					a = a - (b >> i);
					while (a > b) { a = a - b; }
					return a;
				}
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
				return result.withoutLeadingZeros();
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

			ExtraLargeNumber withoutLeadingZeros() {
				int toCut = 0;
				for (int i = getLength() - 1; i >= 0 && get(i) == 0; i--) { toCut++; }
				ExtraLargeNumber result(getLength() - toCut);
				for (int i = 0; i < getLength() - toCut; i++) { result.set(i, get(i)); }
				return result;
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
