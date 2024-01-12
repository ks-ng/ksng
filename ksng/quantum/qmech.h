#include <cmath>
#include <complex>
#include "../util/sda.h"

using namespace std;

#define COMPLEX complex<double>
#define AMP0 COMPLEX(0,0)
#define AMP1 COMPLEX(1,0)
#define PHASE(theta) COMPLEX(cos(theta),sin(theta))
#define PI (double)(3.14159265358979323846)

namespace qmech {

	double ampToProb(COMPLEX z) {
		return pow(abs(z), 2);
	}

	bool isPowOf2(int n) { long long x = 1; while (x < n) { x *= 2; } return x == n; }

	class QuantumVector: public sda::SecureDataArray<COMPLEX> {

		public:

			explicit QuantumVector() {}
			explicit QuantumVector(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// Access already written in SDA

			// Quantum operations

			COMPLEX netAmp() {
				COMPLEX z;
				for (int i = 0; i < getLength(); i++) { z += get(i); }
				return z;
			}

			double netProb() { return ampToProb(netAmp()); }
			void normalize() { double np = netProb(); for (int i = 0; i < getLength(); i++) { set(i, get(i) / np); } }

			QuantumVector operator*(QuantumVector other) {
				QuantumVector result(getLength() * other.getLength());

				int index = 0;
				for (int i = 0; i < getLength(); i++) {
					for (int j = 0; j < other.getLength(); j++) {
						result.set(index, get(i) * other.get(j));
						index++;
					}
				}

				return result;
			}

			// Utility

			sda::SDA<QuantumVector> binaryDecomp(bool autoNormalize=false) {
				int len = getLength();
				int log2len = log2(len);
				if (!isPowOf2(len)) {
					notif::error("cannot provide binary decomposition of non-binary ket");
					return sda::SDA<QuantumVector>(0);
				}

				sda::SDA<QuantumVector> results(log2len);
				for (int i = 0; i < log2len; i++) { results.get(i).initialize(2); }
				QuantumVector qv;
				
				for (int i = 0; i < len; i++) {
					for (int bitIndex = 0; bitIndex < log2len; bitIndex++) {
						qv = results.get(bitIndex);
						int bit = (i >> bitIndex) % 2;
						qv.set(bit, qv.get(bit) + get(i));
					}
				}

				if (autoNormalize) { for (int i = 0; i < log2len; i++) { results.get(i).normalize(); } }

				return results.reverse();
			}

	};

	using QV = QuantumVector;

	class QuantumOperator: public sda::SecureDataMatrix<COMPLEX> {

		public:

			QuantumOperator() {}
			QuantumOperator(int size, bool locked=false, Severity securityLevel=ALERT) { 
				initialize(size, size, locked, securityLevel); 
				reveal();
			}
			QuantumOperator(COMPLEX a00, COMPLEX a01, COMPLEX a10, COMPLEX a11, bool locked=false, Severity securityLevel=ALERT) {
				initialize(2, 2, locked, securityLevel);
				reveal();
				set(0, 0, a00);
				set(0, 1, a01);
				set(1, 0, a10);
				set(1, 1, a11);
			}

			// Access already written in SDM

			// Quantum operations

			QuantumVector operator|(QuantumVector ket) {
				if (getRows() != ket.getLength() || getCols() != ket.getLength()) {
					notif::error("can\'t use operator on differently-sized ket");
					return QuantumVector(ket.getLength());
				} else {
					QuantumVector result(ket.getLength());
					sda::SDA<COMPLEX> row;
					for (int i = 0; i < ket.getLength(); i++) {
						row = getRow(i);
						row.reveal();
						for (int j = 0; j < ket.getLength(); j++) {
							result.set(j, result.get(j) + (ket.get(i) * row.get(j)));
						}
					}
					return result;
				}
			}

	};

	using QO = QuantumOperator;

};
