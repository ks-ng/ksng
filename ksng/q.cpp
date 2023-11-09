#include "utils.cpp"

class Ket {

	public:

		Ket(const int stateCount_) {
			int stateCount = stateCount_;
			complex<double> amps[stateCount];
			double frac = 1 / sqrt(stateCount);
			for (int i = 0; i < stateCount; i++) {
				amps[i] = complex<double>(frac, 0);
			}
		}

		Ket combine(Ket other) {
			Ket result(stateCount * other.stateCount);
			index = 0;
			for (int i = 0; i < stateCount; i++) {
				for (int j = 0; j < other.stateCount; j++) {
					result.amps[index] = amps[i] * other.amps[j];
				}
			}
			return result;
		}

		Ket operator+(Ket other) {
			Ket result(stateCount);
			for (int i = 0; i < stateCount; i++) {
				result[i] = amps[i] + other.amps[i];
			}
			return result
		}

		Ket operator-(Ket other) {
			Ket result(stateCount);
			for (int i = 0; i < stateCount; i++) {
				result[i] = amps[i] - other.amps[i];
			}
			return result
		}

		Ket operator*(Ket other) {
			Ket result(stateCount);
			for (int i = 0; i < stateCount; i++) {
				result[i] = amps[i] * other.amps[i];
			}
			return result
		}

};

class Bra {

	public:

		int state;

		Bra(int state_) {
			state = state_;
		}

		complex<double> operator|(Ket ket) {
			
		}

};
