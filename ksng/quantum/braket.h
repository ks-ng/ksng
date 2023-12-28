#pragma once

#include <cmath>
#include <complex>
#include "../util/sda.h"
#define COMPLEX complex<double>

using namespace std;

#define AMP_NULL COMPLEX(0.0d, 0.0d)
#define AMP_0 COMPLEX(0.0d, 0.0d)
#define AMP_1 COMPLEX(1.0d, 0.0d)

namespace braket {

	inline double ampToProb(COMPLEX amp) {
		return pow(abs(amp), 2);
	}

	string complexRepr(COMPLEX z) {
		if (z == AMP_0) {
			return (string)("0.0");
		}
		stringstream ss;
		if (z.real() != 0.0d) {
			ss << z.real();
		}
		if (z.real() != 0.0d && z.imag() != 0.0d) {
			ss << "+";
		}
		if (z.imag() != 0.0d) {
			ss << z.imag() << "i";
		}
		return ss.str();
	}

	class QuantumVector: public sda::SecureDataArray<COMPLEX> {

		public:

			// Constructors

			QuantumVector() {}
			QuantumVector(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// Quantum methods

			// as a bra
			COMPLEX operator|(QuantumVector ket) {
				COMPLEX sum = AMP_NULL;
				for (int i = 0; i < getLength(); i++) {
					sum += get(i) * ket.get(i);
				}
				return sum;
			}

			inline COMPLEX amp(int i) { return get(i); }
			inline double prob(int i) { return ampToProb(get(i)); }

			COMPLEX netAmp() {
				COMPLEX s = AMP_0;
				for (int i = 0; i < getLength(); i++) {
					s += amp(i);
				}
				return s;
			}

			double netProb() {
				return ampToProb(netAmp());
			}

			void normalize() {
				double inp = sqrt(netProb());
				if (inp == 1.0d) {
					return;
				} else {
					for (int i = 0; i < getLength(); i++) {
						set(i, get(i) / inp);
					}
					return;
				}
			}

			QuantumVector operator*(QuantumVector other) {
				QuantumVector result(getLength() * other.getLength());
				int k = 0;
				for (int i = 0; i < getLength(); i++) {
					for (int j = 0; j < other.getLength(); j++) {
						result.set(k, get(i) * other.get(j));
						k++;
					}
				}
				return result;
			}

			// Utility

			string ampRepr() {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << i << "=" << complexRepr(get(i));
					if (i != getLength() - 1) {
						ss << ", ";
					}
				}
				return ss.str();
			}

			string probRepr() {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << i << "=" << ampToProb(get(i));
					if (i != getLength() - 1) {
						ss << ", ";
					}
				}
				return ss.str();
			}

	};

	using QV = QuantumVector;

	class QuantumOperator: public sda::SecureDataMatrix<COMPLEX> {

		public:

			int size;

			// Constructors

			QuantumOperator() {}
			QuantumOperator(int size, bool locked=false, Severity securityLevel=ALERT): size(size) { initialize(size, size, locked, securityLevel); }

			// Quantum methods

			QuantumVector operate(QuantumVector ket) {
				if (ket.getLength() != size) {
					notif::fatal("cannot use operator on ket of different size");
				}
				QuantumVector result(ket.getLength());
				sda::SDA<COMPLEX> row;
				COMPLEX factor;
				for (int i = 0; i < size; i++) {
					row = getRow(i);
					factor = ket.get(i);
					for (int j = 0; j < ket.getLength(); j++) {
						result.set(j, result.get(j) + factor * row.get(j));
					}
				}
				return result;
			}

			QuantumVector operator|(QuantumVector ket) {
				return operate(ket);
			}

			void applyTo(QuantumVector& ket) {
				ket = operate(ket);
			}
 
	};

	using QO = QuantumOperator;

};