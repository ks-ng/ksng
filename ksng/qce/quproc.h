#pragma once
#include "../util/dstruct/sll.h"
#include "qucircuit.h"

namespace quproc {

	class QuantumRegister {

		public:

			string name;
			qudata::Qubits qubits;

			QuantumRegister() {}
			QuantumRegister(string n, int size): name(n), qubits(qudata::Qubits(size)) {}
			QuantumRegister(string n, initializer_list<qcore::QuantumState> initList): name(n), qubits(qudata::Qubits(initList)) {}

			void route(qucircuit::QuantumCircuit circ) { circ.routeref(qubits); }

	};

	class QuantumLogic {

		private:

			sll::List<qucircuit::QuantumCircuit> quantumRegisters;

		public:

			QuantumProcessor() {}

			void addQuantumRegister(string name, int size) { quantumRegisters.append(QuantumRegister(name, size)); }
			QuantumRegister& getRegister(string name) {
				QuantumRegister qr;
				for (int i = 0; i < quantumRegisters.getLength(); i++) {
					qr = quantumRegisters.get(i);
					if (qr.name == name) {
						return qr;
					}
				}
				stringstream ss;
				ss << "quantum processor: no such register \"" << name << "\"";
				notif::fatal(ss.str());
				return quantumRegisters.get(0);
			}

			void route(qucircuit::QuantumCircuit circ, string name) { getRegister(name).route(circ); }

	};

};