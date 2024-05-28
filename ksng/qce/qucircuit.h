#pragma once
#include "qudata.h"

namespace qucircuit {

    class QuantumGate {

        private:

            sda::Array<int> indices;
            qcore::QO operation;

        public:

			QuantumGate() {}

            QuantumGate(int index, qcore::QuantumOperator op) {
                indices = sda::Array<int>({index});
                operation = op;
            }

            sda::Array<int> getIndices() { return indices; }
            int getIndex(int i = 0) { return indices.get(i); }
			int getSize() { return indices.getLength(); }
            qcore::QO getOperator() { return operation; }

    };

    class QuantumCircuit: public sda::Array<QuantumGate> {

        public:

			QuantumCircuit() {}

			QuantumCircuit(int gateCount) {
				length = gateCount;
				elements = new QuantumGate[length];
				locked = false;
				securityLevel = ALERT;
			}

            QuantumCircuit(initializer_list<QuantumGate> initList) {
				length = initList.size();
				elements = new QuantumGate[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			qudata::Qubits route(qudata::Qubits input) {
				QuantumGate qg;
				int size;
				for (int i = 0; i < getLength(); i++) {
					qg = get(i);
					size = qg.getSize();
					if (size == 1) {
						input.applyOperator(qg.getIndex(), qg.getOperator());
					} else if (size == 2) {
						input.applyDoubleOperator(qg.getIndices(), qg.getOperator());
					} else {
						input.applyMultipleOperator(qg.getIndices(), qg.getOperator());
					}
				}
				return input;
			}

			void routeref(qudata::Qubits& inputref) {
				QuantumGate qg;
				int size;
				for (int i = 0; i < getLength(); i++) {
					qg = get(i);
					size = qg.getSize();
					if (size == 1) {
						inputref.applyOperator(qg.getIndex(), qg.getOperator());
					} else if (size == 2) {
						inputref.applyDoubleOperator(qg.getIndices(), qg.getOperator());
					} else {
						inputref.applyMultipleOperator(qg.getIndices(), qg.getOperator());
					}
				}
			}

    };

};