#include <cmath>
#include <complex>

using namespace std;

#define COMPLEX complex<double>
#define AMP_0 complex<double>(0.0, 0.0)
#define AMP_1 complex<double>(1.0, 0.0)

namespace quantum {

	class Qubit {

		public:

			COMPLEX alpha;
			COMPLEX beta;

			// Constructors

			Qubit() {}
			Qubit(int value) {
				if (value == 1) {
					alpha = AMP_0;
					beta = AMP_1;
				} else {
					alpha = AMP_1;
					beta = AMP_0;
				}
			}

			COMPLEX amp(int value) {
				if (value == 0) {
					return alpha;
				} else if (value == 1) {
					return beta;
				} else {
					return 0;
				}
			}

			double probability(int value) {
				if (value == 0) {
					return pow(abs(alpha), 2);
				} else if (value == 1) {
					return pow(abs(beta), 2);
				} else {
					return 0.0d;
				}
			}

	};

};