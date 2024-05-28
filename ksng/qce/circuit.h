#pragma once
#include "qudata.h"

namespace circuit {

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
            int getIndex(int i) { return indices.get(i); }
            qcore::QO getOperator() { return operation; }

    };

    class QuantumCircuit: public sda::Array<QuantumGate> {

        public:

			QuantumCircuit() {}

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

    };

};