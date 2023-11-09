#include "utils.cpp"

class Ket {

	/*
 	Kets represent quantum superpositions of
  	binary numbers.

	Each ket is associated with a decimal number
 	which is encoded by its qubits. Applying a bra
  	to a ket returns the probability of the bits
   	acquired from measuring the qubits encoding
    	the bra's decimal number.
	*/

	public:

		int bitlength;
		int size;

		QuantumState(long long value, int bitlength_) {
			bitlength = bitlength_;
			size = pow(2, bitlength)
			complex<double> amps[size];
			
			for (long long i = 0; i < size; i++) {
				if (i == value) {
					amps[i] = complex<double>(1.0, 0.0);
				} else {
					amps[i] = complex<double>(0.0, 0.0);
				}
			}
		}

		double amplitude(int value) {
			return amps[value];
		}

		double probability(int value) {
			return pow(abs(amps[value]), 2);
		}

		Ket operator*(Ket other) {
			Ket result(-1, bitlength * other.bitlength);
			for (long long i = 0; i < size; i++) {
				for (long long j = 0; j < other.size; j++) {
					result.amps = amps[i] * amps[j];
				}
			}
			return result;
		}

};

class Bra {

	public:

		long long value;

		Bra(long long value_) {
			value = value_;
		}

		complex<double> operator|(Ket state) {
			return state.amplitude(value);
		}

};

complex<double> innerProduct(Bra bra, Ket ket) {
	return /*<*/ bra | ket /*>*/ ;
}
