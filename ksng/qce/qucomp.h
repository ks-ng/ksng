#pragma once
#include "../util/dstruct/sll.h"
#include "qucircuit.h"

namespace qucomp {

	class QuantumRegister {

		public:

			string name;
			qudata::Qubits qubits;

			QuantumRegister(string n, int size): name(n), qubits(qudata::Qubits(size)) {}
			QuantumRegister(string n, initializer_list<qcore::QuantumState> initList): name(n), qubits(qudata::Qubits(initList)) {}

	};

	class QuantumComputer {

		private:

			sll::List<qucircuit::QuantumCircuit> quantumRegisters;

		public:

			QuantumComputer() {}

			void addQuantumRegister(string name, int size) { quantumRegisters.append(QuantumRegister(name, size)); }

	};

};