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

class OuterProduct {



}
