#pragma once

#include "braket.h"
#include <cstdlib>
#include <ctime>

using namespace std;

namespace qubit {

	class Qubit {

		public:

			braket::QV vec = braket::QV(2);

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

			void force(int i) {
				if (i == 1) {
					vec.set(0, AMP_0);
					vec.set(1, AMP_1);
				} else {
					vec.set(0, AMP_1);
					vec.set(1, AMP_0);
				}
				normalize();
			}

			COMPLEX get(int i) {
				return vec.get(i);
			}

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

			void set(COMPLEX alpha, COMPLEX beta) {
				vec.set(0, alpha);
				vec.set(1, beta);
			}

			void set(braket::QuantumVector qv) {
				vec = qv;
			}

			// Quantum methods

			void applyOperator(braket::QuantumOperator op) {
				if (op.getRows() != 2 || op.getCols() != 2) {
					notif::fatal("invalid operator for single qubit");
				}

				braket::QuantumVector result = op | vec;
				vec.set(0, result.get(0));
				vec.set(1, result.get(1));
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

			string repr() {
				return braket::complexRepr(vec.get(0)) + (string)("|0> + ") + braket::complexRepr(vec.get(1)) + (string)("|1>");
			}

	};

	class QuantumRegister: public sda::SecureDataArray<Qubit> {

		public:

			QuantumRegister() {}
			QuantumRegister(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			// Quantum methods

			void applyOperator(int i, braket::QuantumOperator op) {
				get(i).applyOperator(op);
				get(i).normalize();
			}

			void applyControlledOperator(int target, int control, braket::QuantumOperator op) {
				COMPLEX control0 = get(control).get(0);
				COMPLEX control1 = get(control).get(1);
				op.set(0, 0, op.get(0, 0) * control1);
				op.set(0, 1, op.get(0, 1) * control0);
				op.set(1, 0, op.get(1, 0) * control0);
				op.set(1, 1, op.get(1, 1) * control1);
				applyOperator(target, op);
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

			void normalize() {
				for (int i = 0; i < getLength(); i++) {
					get(i).normalize();
				}
			}	

			// Utility

			string qubitRepr() {
				stringstream ss;
				ss << "<";
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

};