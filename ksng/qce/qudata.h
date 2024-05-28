#pragma once
#include <random>
#include <cmath>
#include "qcore.h"

namespace qudata {

	qcore::QuantumState mkqb(int v) {
		if (v == 0) {
			return qcore::QuantumState({1, 0});
		}
		return qcore::QuantumState({0, 1});
	}

	class Qubits: public sda::Array<qcore::QuantumState> {

		public:

			Qubits(initializer_list<qcore::QS> initList) {
				length = initList.size();
				elements = new qcore::QS[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			Qubits(initializer_list<int> initList) {
				length = initList.size();
				elements = new qcore::QS[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = mkqb(val);
				}
			}

			Qubits(int x, int mbl = 0) {
				int l2l = max(ceil(log2(x)), (double)(mbl));
				initialize(l2l);
				for (int i = 0; i < l2l; i++) {
					set(i, mkqb(x & (1 << i)));
				}
			}

			void applyOperator(int index, qcore::QuantumOperator op) {
				set(index, op | get(index));
			}

			void applyDoubleOperator(int a, int b, qcore::QO op) {
				// slightly more optimal than applyMultipleOperator
				qcore::QS state = get(a) * get(b);
				sda::Array<qcore::QS> data = (op | state).extractQuantumData();
				set(a, data.get(0));
				set(b, data.get(0));
			}

			void applyMultipleOperator(sda::Array<int> indices, qcore::QO op) {
				qcore::QS state = get(indices.get(0));
				for (int i = 1; i < indices.getLength(); i++) {
					state = state * get(indices.get(i));
				}
				sda::Array<qcore::QS> data = (op | state).extractQuantumData();
				for (int i = 0; i < data.getLength(); i++) {
					set(indices.get(i), data.get(i));
				}
			}

	};

};