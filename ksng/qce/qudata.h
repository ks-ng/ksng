#pragma once

#include "qcore.h"

namespace qubits {

	inline qcore::QuantumState qubit0() { return qcore::QuantumState({1, 0}); }
	inline qcore::QuantumState qubit1() { return qcore::QuantumState({0, 1}); }
	qcore::QuantumState qubit(int x) { if (x == 0) { return qubit0(); } else { return qubit1(); } }

	class Qubit: public qcore::QuantumState {

		public:

			Qubit() {}
			Qubit(int x) {}

	};

	class Qubits: public sda::Array<qcore::QuantumState> {

		public:

            Qubits() {}
			Qubits(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			Qubits(initializer_list<qcore::QuantumState> initList) {
				length = initList.size();
				elements = new qcore::QuantumState[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			Qubits(initializer_list<int> initList) {
				length = initList.size();
				elements = new qcore::QuantumState[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = qubit(val);
				}
			}

			Qubits(int x) {
				length = ceil(log2(x));
				elements = new qcore::QuantumState[length];
				locked = false;
				securityLevel = ALERT;
				bool s;
				for (int i = 0; i < length; i++) {
					s = 1 == ((x >> i) & 1)
					elements[i] = qubit(s);
					// possibly overdefined - maybe qubit((x >> i) & 1) would work
				}
			}

			Qubits(int x, int maxBitLength) {
				length = maxBitLength;
				elements = new qcore::QuantumState[length];
				locked = false; 
				securityLevel = ALERT;
				bool s;
				for (int i = 0; i < length; i++) {
					s = 1 == ((x >> i) & 1)
					elements[i] = qubit(s);
				}
			}

			void normalize() {
				for (int i = 0; i < getLength(); i++) {
					get(i).normalize();
				}
			}

	};

	class Qubyte: public Qubits {

		public:

			Qubyte() {}
			Qubyte(int x) {
				if (0 <= x < 256) {

				}
			}

	}

};