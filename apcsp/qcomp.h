#include "qmech.h"
#include <random>

using namespace std;

namespace qcomp {

	class Qubit {

		public:

			// Constructors

			qmech::QuantumVector vec;

			void setupVector() { vec.initialize(2); }

			Qubit() { setupVector(); }
			Qubit(qmech::QuantumVector ket): vec(ket) {
				if (ket.getLength() != 2) {
					notif::fatal("cannot initialize a qubit on a ket that doesn\'t have exactly two eigenstates");
				}
			}
			Qubit(int value, double phase=0) {
				setupVector();
				initialize(value, phase);
			}

			void initialize(int value, double phase=0) {
				if (value == 1) {
					vec.set(0, AMP0);
					vec.set(1, PHASE(phase));
				} else {
					vec.set(0, PHASE(phase));
					vec.set(1, AMP0);
				}
			}

			// Access

			qmech::QuantumVector& getVector() { return vec; }

			// Quantum operations

			COMPLEX amp(int value) { if (value == 1) { return vec.get(1); } else { return vec.get(0); } }
			double prob(int value) { return qmech::ampToProb(amp(value)); }

			int safeCollapse() { if ((double)(rand()) / RAND_MAX >= prob(1)) { return 1; } else { return 0; } }
			int collapse() { 
				int result = safeCollapse(); 
				if (result == 1) {
					vec.set(0, AMP0);
					vec.set(1, AMP1);
					return 1;
				} else {
					vec.set(0, AMP1);
					vec.set(1, AMP0);
					return 0;
				}
			}

			string repr() {
				stringstream ss;
				ss << "<" << amp(0) << "|0⟩+" << amp(1) << "|1⟩>";
				return ss.str();
			}

	};

	using Q = Qubit;

	class QuantumGate {

		public:

			qmech::QuantumOperator op;
			bool ctrl;
			int target;
			int control;

			QuantumGate() {}
			QuantumGate(qmech::QO op_, int target, int control=-1): ctrl((bool)(control > 0)), target(target), control(control) {
				if (ctrl && (op.getRows() != 4 || op.getCols() != 4)) {
					op = qmech::controlled(op_);
				} else {
					op = op_;
				}
			}

	};

	class QuantumCircuit: public sda::SecureDataArray<QuantumGate> {

		public:

			QuantumCircuit() {}
			QuantumCircuit(int size, bool locked, Severity securityLevel) { initialize(size, locked, securityLevel); }

	};

	using QG = QuantumGate;

	class QuantumRegister: public sda::SecureDataArray<Qubit> {

		public:

			QuantumRegister() {}
			QuantumRegister(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// Access built-in

			// Quantum operations

			sda::SDA<int> safeCollapse() { 
				sda::SDA<int> result(getLength());
				for (int i = 0; i < getLength(); i++) { 
					result.set(i, get(i).safeCollapse()); 
				}
				return result;
			}

			sda::SDA<int> collapse() { 
				sda::SDA<int> result(getLength());
				for (int i = 0; i < getLength(); i++) { 
					result.set(i, get(i).collapse()); 
				}
				return result;
			}

			void applyOperator(int target, qmech::QuantumOperator op) {
				if (op.getRows() != 2 || op.getCols() != 2) { notif::fatal("operator is incorrect size"); }

				set(target, Qubit(op | get(target).getVector()));
			}

			void applyDoubleOperator(int a, int b, qmech::QuantumOperator op) {
				if (op.getRows() != 4 || op.getCols() != 4) { notif::fatal("operator is incorrect size"); }
				qmech::QV vec = get(b).getVector() * get(a).getVector();
				vec = op | vec;
				sda::SDA<qmech::QV> result = vec.binaryDecomp();
				
				qmech::QV bf = result.get(0);
				qmech::QV af = result.get(1);
				set(b, Qubit(bf));
				set(a, Qubit(af));
			}

			void applyTripleOperator(int a, int b, int c, qmech::QuantumOperator op) {
				if (op.getRows() != 8 || op.getCols() != 8) { notif::fatal("operator is incorrect size"); }
				qmech::QV vec = get(c).getVector() * get(b).getVector() * get(a).getVector();
				vec = op | vec;
				sda::SDA<qmech::QV> result = vec.binaryDecomp();

				qmech::QV cf = result.get(0);
				qmech::QV bf = result.get(1);
				qmech::QV af = result.get(2);
				set(c, Qubit(cf));
				set(b, Qubit(bf));
				set(a, Qubit(af));
			}

			void applyControlledOperator(int target, int control, qmech::QuantumOperator op) {
				applyDoubleOperator(target, control, qmech::controlled(op));
			}

			// Utility

			string qubitRepr() {
				stringstream ss;
				ss << "<QR ";
				for (int i = 0; i < getLength(); i++) {
					ss << get(i).repr();
					if (i != getLength() - 1) {
						ss << ", ";
					}
				}
				ss << ">";
				return ss.str();
			}

	};

	using QR = QuantumRegister;

};

namespace qstd {

	qmech::QO PauliX(bool ctrl=false) { qmech::QO px(0, 1, 1, 0); return (ctrl? controlled(px): px); }
	qmech::QO PauliY(bool ctrl=false) { qmech::QO py(0, COMPLEX(0, -1), COMPLEX(0, 1), 0); return (ctrl? controlled(py): py); }
	qmech::QO PauliZ(bool ctrl=false) { qmech::QO pz(1, 0, 0, -1); return (ctrl? controlled(pz): pz); }
	qmech::QO PhaseShift(double phase, bool ctrl=false) { qmech::QO ps(1, 0, 0, exp(COMPLEX(0, 1) * phase)); return (ctrl? controlled(ps): ps); }
	qmech::QO Hadamard(bool ctrl=false) { qmech::QO h(1/sqrt(2), 1/sqrt(2), 1/sqrt(2), -1/sqrt(2)); return (ctrl? controlled(h): h); }

};