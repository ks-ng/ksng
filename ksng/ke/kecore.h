#pragma once

#include <functional>
#include <cmath>

#include "../util/dstruct/sll.h"
#include "../util/dstruct/sda.h"

using namespace std;

namespace kecore {

	class Entity: public sda::Array<double> {

		public:

			Entity() {}
			Entity(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }
			Entity(initializer_list<double> initList) {
				length = initList.size();
				elements = new double[length];
				locked = false; // By default, not locked when initialized with a list
				securityLevel = ALERT; // Default security level
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			double operator^(Entity other) {
				double s = 0;
				for (int i = 0; i < getLength() && i < other.getLength(); i++) {
					s += pow(get(i) - other.get(i), 2);
				}
				return sqrt(s);
			}

	};

	class Event {

		public:

			Entity initialState;
			Entity takenAction;
			Entity finalState;

			Event(Entity i, Entity a, Entity f): initialState(i), takenAction(a), finalState(f) { }

	};

	class Transactor {

		private:

			using TransactionFunctionType = function<Entity(Entity, Entity)>;

			TransactionFunctionType transaction;
			Entity currentState;

		public:

			Transactor() {}
			Transactor(TransactionFunctionType t, Entity initialState): transaction(t), currentState(initialState) {}

			Entity getState() { return currentState; }

			Entity transact(Entity action) {
				currentState = transaction(currentState, action);
				return currentState; // I'm assuming that transaction will always be defined, as the Transactor won't initialize without it
			}

	};

	class Directive {

		private:

			Entity state;
			double priority; // high priority = more valuable

		public:

			Directive() {}
			Directive(Entity s, double p): state(s), priority(p);

			double reward

	};

};