#include <cmath>
#include <complex>
#include "../util/sda.h"

using namespace std;

#define COMPLEX complex<double>
#define AMP0 COMPLEX(0,0)
#define AMP1 COMPLEX(1,0)

namespace qsys {

	class QuantumVector: public sda::SecureDataArray<COMPLEX> {

		public:

			explicit QuantumVector() {}
			explicit QuantumVector(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// Access already written in SDA

			// Quantum operations

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

			QuantumVector operator|(QuantumVector ket) {
				if (getLength() != ket.getLength()) {
					notif::fatal("can\'t mix differently-sized bras and kets");
					return QuantumVector(0);
				}
				COMPLEX result = AMP0;
				
			}

	};

};
