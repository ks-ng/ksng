#include "qcomp.h"

namespace qstd {

	complex<double> im(0.0, 1.0);
	double sqrt2 = sqrt(2);
	double invsqrt2 = 1 / sqrt2;
	complex<double> AMPsqrt2(sqrt2, 0);
	complex<double> AMPinvsqrt2(invsqrt2, 0);

	using namespace qcore;
	using namespace qcomp;

	inline QuantumOperator GlobalPhase(double phase) {
		AMPLITUDE iphase = exp(im * AMPLITUDE(phase));
		QO r = {{iphase, AMP0},
				{AMP0, iphase}};
		return r;
	}

	inline QuantumOperator PauliX() {
		QO r = {{AMP0, AMP1},
				{AMP1, AMP0}};
		return r;
	}

	inline QuantumOperator NOT() { return PauliX(); }

	inline QuantumOperator PauliY() {
		QO r = {{AMP0, -im},
				{im, AMP0}};
		return r;
	}

	inline QuantumOperator PauliZ() {
		QO r = {{AMP1, AMP0},
				{AMP0, -AMP1}};
		return r;
	}

	inline QuantumOperator SPhase() {
		QO r = {{AMP1, AMP0},
				{AMP0, im}};
		return r;
	}

	inline QuantumOperator SqrtZ() { return SPhase(); } 

	inline QuantumOperator SqrtX() {
		QO r = {{AMPLITUDE(1, 1) / AMPLITUDE(2, 0),  AMPLITUDE(1, -1) / AMPLITUDE(2, 0)},
				{AMPLITUDE(1, -1) / AMPLITUDE(2, 0), AMPLITUDE(1, 1) / AMPLITUDE(2, 0)}};
		return r;
	}

	inline QuantumOperator SqrtNOT() { return SqrtX(); }

	inline QuantumOperator Hadamard() {
		QO r = {{AMPinvsqrt2, AMPinvsqrt2},
				{AMPinvsqrt2, -AMPinvsqrt2}};
		return r;
	}

	inline QuantumOperator H() { return Hadamard(); }

}
