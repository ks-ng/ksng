#include "utils.cpp"

template <int bitlength>
class Ket {

	public:

		long long length = pow(2, bitlength);

		complex<double> amplitudes[size];

		QuantumState(long long value) {
			for (long long i = 0; i < size; i++) {
				if (i == value) {
					amplitudes[i] = complex<double>(1.0, 0.0);
				} else {
					amplitudes[i] = complex<double>(0.0, 0.0);
				}
			}
		}

}
