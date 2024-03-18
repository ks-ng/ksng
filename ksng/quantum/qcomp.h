#pragma once
#include "qcore.h"
#include "../util/data.h"

namespace qcomp {

    inline qcore::QuantumVector Qubit(qcore::AMPLITUDE alpha, qcore::AMPLITUDE beta) {
        qcore::QV result = {alpha, beta};
        return result;
    }

    inline qcore::QuantumVector Qubit(int value) {
        if (value == 0) {
            qcore::QV result = {AMP1, AMP0};
            return result;
        } else {
            qcore::QV result = {AMP0, AMP1};
            return result;
        }
    }

    class Qubits: public sda::SecureDataArray<qcore::QuantumVector> {

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
                qcore::QV v = {AMP1};
                bool firstDone = false;
                for (const auto& index : targetQubits) {
                    if (firstDone) {
                        v = v * get(index);
                    } else {
                        v = get(index);
                        firstDone = true;
                    }
                }
                v = (op | v);
                sda::SDA<qcore::QV> quantumData = v.extractQuantumData();
                int i = 0;
                for (const auto& index : targetQubits) {
                    set(index, quantumData.get(i));
                    i++;
                }
            }

            void applyOperator(int targetQubit, qcore::QO op) {
                set(targetQubit, op | get(targetQubit));
            }

            static Qubits classicalToQuantum(data::Bits classical) {
                Qubits result(classical.getLength());
                for (int i = 0; i < classical.getLength(); i++) {
                    result.set(i, Qubit(classical.get(i)));
                }
                return result;
            }

            static Qubits classicalToQuantum(data::Bytes classical) { return Qubits::classicalToQuantum(data::bytesToBits(classical)); }

    };

    bool WARNED_FOR_INDETERMINACY = false;

    data::Bits qubitsToBits(Qubits qr) {
        if (!WARNED_FOR_INDETERMINACY) {
            WARNED_FOR_INDETERMINACY = true;
            notif::warning("quantum data is not determinate and may yield different results each time measurements are taken");
        }
        data::Bits result(qr.getLength());
        for (int i = 0; i < qr.getLength(); i++) {
            result.set(i, qr.get(i).collapse());
        }
        return result;
    }

    inline data::Bytes qubitsToBytes(Qubits qr) {
        return data::bitsToBytes(qubitsToBits(qr));
    }

    inline Qubits bitsToQubits(data::Bits classical) { return Qubits::classicalToQuantum(classical); }
    inline Qubits bytesToQubits(data::Bytes classical) { return Qubits::classicalToQuantum(classical); }

};