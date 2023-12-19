#include <cmath>
#include <complex>
#include "util/notif.h"

#define COMPLEX complex<double>
#define AMP_ON complex<double>(1, 0)
#define AMP_OFF complex<double>(0, 0)

using namespace std;

class State {

	public:

		int size;
		COMPLEX* amps;

		State(int size): size(size), amps(new COMPLEX[size]) {
			amps[0] = AMP_ON;
			for (int i = 1; i < size; i++) {
				amps[i] = AMP_OFF;
			}
		}

		inline COMPLEX& amp(int index) { return amps[index]; }
		inline double getProbability(int index) { return norm(amp(index)); }
		inline int mostProbable() {
			int mostProbableIndex = 0;
			int highestProbability = getProbability(0);

			for (int i = 1; i < size; i++) {
				int newProbability = getProbability(i);
				if (newProbability > highestProbability) {
					mostProbableIndex = i;
					highestProbability = newProbability;
				}
			}

			return mostProbableIndex;
		}

		COMPLEX operator|(State ket) {
			// this state is considered to be a bra
			// while the other is considered to be a ket
			// so the syntax is "bra | ket"

			if (size != ket.size) {
				notif::error("can\'t multiply bras and kets of different lengths");
				return AMP_OFF;
			}

			COMPLEX s = 0;
			for (int i = 0; i < size; i++) {
				s += conj(amp(i)) * ket.amp(i);
			}
			return s;
		}

};

class Operator {

	public:

		int size;
		COMPLEX** rows;

		Operator(int size): size(size), rows(new COMPLEX*[size]) {
			for (int i = 0; i < size; i++) {
				rows[i] = new COMPLEX[size];
			}
		}

		COMPLEX getItem(int row, int col) {
			if (row >= size || col >= size) {
				notif::error("row/col out of range for Operator");
				return AMP_OFF;
			}
		}

		State operator|(State ket) {
			if (size != ket.size) {
				notif::fatal("can't operate on a ket of different size than operator");
			}

			State result(ket.size);
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					result.amp(i) += ket.amp(i) * getItem(i, j);
				}
			}

			return result;
		}

};

class QuantumRegister {

	public:

		int size;
		State* qubits;

		

};