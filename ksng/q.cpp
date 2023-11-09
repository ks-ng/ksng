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

		Ket operator*(Ket other) {
			Ket result(stateCount * other.stateCount);
			index = 0;
			for (int i = 0; i < stateCount; i++) {
				for (int j = 0; j < other.stateCount; j++) {
					result.amps[index] = amps[i] * other.amps[j];
				}
			}
			return result;
		}

};
