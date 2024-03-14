#pragma once
#include "qcore.h"

namespace qcomp {

    inline qcore::QuantumVector Qubit(qcore::AMPLITUDE alpha, qcore::AMPLITUDE beta) {
        qcore::QV result = {alpha, beta};
        return result;
    }

    class Qubits: sda::SecureDataArray<qcore::QuantumVector> {

        public:

            Qubits() {}
			Qubits(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			Qubits(initializer_list<qcore::QV> initList) {
				length = initList.size();
				elements = new qcore::QV[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

            string intStates(string delimiter=(string)(" ")) {
                stringstream ss;
                for (int i = 0; i < getLength(); i++) {
                    ss << get(i).intStates();
                    if (i < getLength() - 1) { ss << delimiter; }
                }
                return ss.str();
            }

            string binStates(string delimiter=(string)(" ")) {
                stringstream ss;
                for (int i = 0; i < getLength(); i++) {
                    ss << get(i).binStates();
                    if (i < getLength() - 1) { ss << delimiter; }
                }
                return ss.str();
            }

            void applyOperator(initializer_list<int> targetQubits, qcore::QuantumOperator op) {
                qcore::QV v;
                bool firstDone = false;
                for (const auto& index : targetQubits) {
                    if (firstDone) {
                        v = v * get(index);
                    } else {
                        v = get(index);
                    }
                }
                v = (op | v);
                sda::SDA<qcore::QV> quantumData = v.extractQuantumData();
                for (const auto& index : targetQubits) {
                    set(index, quantumData.get(index));
                }
            }

            void applyOperator(int targetQubit, qcore::QO op) {
                set(targetQubit, op | get(targetQubit));
            }

    };

};