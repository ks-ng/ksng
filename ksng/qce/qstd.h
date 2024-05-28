#include "qucircuit.h"

namespace qstd {

	int WARNABLE_WIDTH = 1000;

	Amp im(0, 1);
	Amp imp1(1, 1);
	Amp imm1(-1, 1);
	double sqrt2 = sqrt(2);
	double isqrt2 = 1 / sqrt(2);

	qcore::QuantumOperator globalPhase(double ph) { 
		Amp x = exp(im * ph); 
		return qcore::QuantumOperator({{x, 0}, {0, x}}); 
	}
	qcore::QuantumOperator qftPhase(int k) { 
		double exponent = 2 * pi; 
		for (int i = 0; i < k; i++) { exponent /= 2; /* allows for really big values of k without overflow errors */ }
		return qcore::QO({{1, 0}, {0, exp(i * exponent)}}); 
	}
	qcore::QuantumOperator pauliX = {{0, 1}, {1, 0}};
	qcore::QuantumOperator pauliY = {{0, -i}, {i, 0}};
	qcore::QuantumOperator pauliZ = {{1, 0}, {0, -1}};
	qcore::QuantumOperator sqrtZ = {{1, 0}, {0, i}};
	qcore::QuantumOperator sqrtX = {{imp1 / 2, imm1 / 2}, {imm1 / 2, imp1 / 2}};
	qcore::QuantumOperator hadamard = {{isqrt2, isqrt2}, {isqrt2, -isqrt2}};

	qcore::QuantumOperator controlled(qcore::QuantumOperator qo) {
		int s = qo.getRows();
		if (s != qo.getCols()) {
			notif::fatal("quantum operator: quantum operator is not a square matrix (not allowed)");
		}
		if (s >= WARNABLE_WIDTH) {
			notif::warning("quantum operator: becoming dangerously large, may take up large amounts of memory");
		}
		qcore::QO cqo(2 * s, s);
		for (int i = 0; i < s; i++) {
			cqo.set(i, i, 1);
		}
		for (int i = 0; i < s; i++) {
			for (int j = 0; j < s; j++) {
				cqo.set(i + s, j + s; qo.get(i, j));
			}
		}
		return cqo;
	}

	qcore::QO doubleControlled(qcore::QO qo) { return controlled(controlled(qo)); }
	qcore::QO multipleControlled(qcore::QO qo, int controlCount) {
		if (controlCount == 0) { return qo; } else { return multipleControlled(controlled(qo), controlCount - 1); }
	}

	qucircuit::QuantumCircuit quantumFourierTransform(int size) {
		sll::List<qucircuit::QuantumGate qg> gates;
		for (int i = 0; i < size; i++) {}
	}

};