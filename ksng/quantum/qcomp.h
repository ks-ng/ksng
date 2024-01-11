#include "qmech.h"
#include <random>

namespace qcomp {

	class Qubit {

		public:

			// Constructors

			qmech::QuantumVector vec;

			Qubit() {}
			Qubit(qmech::QuantumVector ket): vec(ket) {
				if (ket.getLength() != 2) {
					notif::fatal("cannot initialize a qubit on a ket that doesn\'t have exactly two eigenstates");
				}
			}
			Qubit(int value, double phase=0) {
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
				if (op.getRows() != 2 || op.getCols() != 2) {
					notif::fatal("operator is incorrect size");
				}

				set(target, Qubit(op | get(target).getVector()));
			}

			void applyControlledOperator(int target, int control, qmech::QuantumOperator op) {
				qmech::QV vec = get(control).getVector() * get(target).getVector();
				vec = op | vec;
				sda::SDA<qmech::QV> result = vec.binaryDecomp();
				
				qmech::QV ctrl = result.get(0);
				qmech::QV trgt = result.get(1);
				set(control, Qubit(ctrl));
				set(target, Qubit(trgt));
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