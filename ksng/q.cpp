#include "utils.cpp"

class Ket {

	public:

		int stateCount;
		complex<double>* amps;

		Ket(const int stateCount_) {
			int stateCount = stateCount_;
			amps = new complex<double>[stateCount];
			double frac = 1 / sqrt(stateCount);
			for (int i = 0; i < stateCount; i++) {
				amps[i] = complex<double>(frac, 0);
			}
		}

		double netProbability() {
			double ampsum = 0.0;
			for (int i = 0; i < stateCount; i++) {
				ampsum += pow(abs(amps[i]), 2);
			}
			return ampsum;
		}

		void normalize() {
			complex<double> np(netProbability(), 0.0);
			for (int i = 0; i < stateCount; i++) {
				amps[i] = amps[i] / np;
			}
		}

		Ket entangle(Ket other) {
			Ket result(stateCount * other.stateCount);
			int index = 0;
			for (int i = 0; i < stateCount; i++) {
				for (int j = 0; j < other.stateCount; j++) {
					result.amps[index] = amps[i] * other.amps[j];
					index++;
				}
			}
			return result;
		}

		Ket operator+(Ket other) {
			Ket result(stateCount);
			for (int i = 0; i < stateCount; i++) {
				result.amps[i] = amps[i] + other.amps[i];
			}
			return result;
		}

		Ket operator-(Ket other) {
			Ket result(stateCount);
			for (int i = 0; i < stateCount; i++) {
				result.amps[i] = amps[i] - other.amps[i];
			}
			return result;
		}

		Ket operator*(Ket other) {
			Ket result(stateCount);
			for (int i = 0; i < stateCount; i++) {
				result.amps[i] = amps[i] * other.amps[i];
			}
			return result;
		}

};

class Bra {

	public:

		int state;

		Bra(int state_) {
			state = state_;
		}

		complex<double> operator|(Ket ket) {
			return ket.amps[state];
		}

};

class Operator {

	public:

		Matrix* matrixptr;

		Operator(Matrix* matrix_) {
			matrixptr = matrix_;
		}

		Matrix matrix() {
			return *matrixptr;
		}

		Ket operator|(Ket ket) {
			Ket result(ket.stateCount);

			// Apply the operator to the ket

			return result;
		}

};