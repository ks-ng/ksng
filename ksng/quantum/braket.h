#include <cmath>
#include <complex>
#define COMPLEX complex<double>

using namespace std;

#define AMP_NULL COMPLEX(0.0d, 0.0d)
#define AMP_0 COMPLEX(1.0d, 0.0d)
#define AMP_1 COMPLEX(0.0d, 1.0d)

namespace braket {

	class QuantumVector {

		public:

			int size;
			COMPLEX* amps;

			// Constructors

			QuantumVector() {}
			QuantumVector(int size_) {
				initialize(size_);
			}

			void initialize(int size_) {
				size = size_;
				amps = new COMPLEX[size];
			}

			COMPLEX amp(int i) {
				return amps[i];
			}

			double probability(int i) {
				return pow(abs(amp(i)), 2);
			}

			// Quantum methods

			// As a bra
			COMPLEX operator|(QuantumVector ket) {
				COMPLEX result = AMP_NULL;
				for (int i = 0; i < size; i++) {
					result += amp(i) * ket.amp(i);
				}
				return result;
			}

	};

};