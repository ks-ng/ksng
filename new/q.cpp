#include <cmath>
#include <complex>

using namespace std;

int inv(int val) {
	if (val == 1) {
		return 0;
	} else if (val == 0) {
		return 1;
	} else {
		return -1;
	}
}

class Qubit {

	public:

		complex<double> alpha;
		complex<double> beta;

		Qubit(): alpha(complex<double>(1, 0)), beta(complex<double>(0, 0)) {}
		Qubit(int val): alpha(complex<double>(inv(val), 0)), beta(complex<double>(val, 0)) {}

		complex<double> getByIndex(int index) {
			if (index == 0) {
				return alpha;
			} else if (index == 1) {
				return beta;
			} else {
				return complex<double>(0, 0);
			}
		}

		complex<double> operator[](int index) {
			return getByIndex(index);
		}

		int Pr(int index) {
			return pow(getByIndex(index), 2);
		}

};

class QuantumRegister {

	public:

		Qubit* qubits;



};