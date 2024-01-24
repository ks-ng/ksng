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

};
