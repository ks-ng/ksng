#pragma once

#include "braket.h"
#include <cstdlib>
#include <ctime>

using namespace std;

namespace qubit {

	class Qubit {

		public:

			braket::QuantumVector vec(2);

			// Constructors

			Qubit(int value=0) {
				if (value == 1) {
					vec.set(0, AMP_0);
					vec.set(1, AMP_1);
				} else {
					vec.set(0, AMP_1);
					vec.set(1, AMP_0);
				}
				normalize();
			}

			// Access methods

			void set(int i) {
				if (i == 1) {
					vec.set(0, AMP_0);
					vec.set(1, AMP_1);
				} else {
					vec.set(0, AMP_1);
					vec.set(1, AMP_0);
				}
				normalize();
			}

			// Quantum methods

			void applyOperator(braket::QuantumOperator op) {
				if (op.getRows() != 2 || op.getCols() != 2) {
					notif::fatal("invalid operator for single qubit");
				}

				vec = op | vec;
				normalize();
			}

			int safeCollapse() {
				double p1 = vec.prob(1);
				srand(static_cast<unsigned int>(time(0)));
				double r = static_cast<double>(rand()) / RAND_MAX;
				if (r >= p1) {
					return 1;
				} else {
					return 0;
				}
				normalize();
			}

			int collapse() {
				int value = safeCollapse();
				set(value);
				return value;
				normalize();
			}

			void normalize() {
				vec.normalize();
			}

	};

	class QuantumRegister: sda::SecureDataArray<Qubit> {

		public:

			QuantumRegister() {}
			QuantumRegister(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// Quantum methods

			void applyOperator(int i, braket::QuantumOperator op) {
				get(i).applyOperator(op);
				getref(i).normalize();
			}

			void applyControlledOperator(int target, int control, braket::QuantumOperator op) {
				braket::QV system = get(control) * get(target);
				system = op | system;
				getref(target).vec.set(0, system.get(0) + system.get(2));
				getref(target).vec.set(1, system.get(1) + system.get(3));
				getref(target).normalize();
			}

			sda::SecureDataArray<int> safeCollapse() {
				sda::SDA<int> result(getLength());
				for (int i = 0; i < getLength(); i++) {
					result.set(i, get(i).safeCollapse());
				}
				return result;
			}

			sda::SecureDataArray<int> collapse() {
				sda::SDA<int> result(getLength());
				for (int i = 0; i < getLength(); i++) {
					result.set(i, get(i).collapse());
				}
				return result;
			}

	};

};

namespace qstd {

	braket::QuantumOperator PauliX(2);
	PauliX.set(0, 0, 0);
	PauliX.set(0, 1, 1);
	PauliX.set(1, 0, 1);
	PauliX.set(1, 1, 0);

};