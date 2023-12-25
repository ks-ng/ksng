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

			QuantumVector() {}
			QuantumVector(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// as a bra
			COMPLEX operator|(QuantumVector ket) {
				COMPLEX sum = AMP_NULL;
				for (int i = 0; i < getLength(); i++) {
					sum += get(i) * ket.get(i);
				}
				return sum;
			}

			inline COMPLEX amp(int i) { return get(i) }
			inline double prob(int i) { return ampToProb(get(i)); }

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

};