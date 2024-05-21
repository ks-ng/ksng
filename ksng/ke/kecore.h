#pragma once

#include <functional>
#include <cmath>

#include "../util/dstruct/sll.h"
#include "../util/dstruct/sda.h"

using namespace std;

namespace kecore {

	double ZERO_DISTANCE_REWARD = 100;

	namespace core {

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

				const double operator^(Entity other) {
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

	};

	namespace doctrine {

		class Doctrine {

			private:

				using DoctrineFunctionType = function<double(core::Entity, core::Entity)>;

				DoctrineFunctionType doctrine;
				double priority;
				
			public:

				Doctrine() {}
				Doctrine(DoctrineFunctionType d, double p): doctrine(d), priority(p) {}

				double modify(core::Entity state, core::Entity action) {
					return doctrine(state, action) * priority;
				}

		};

		class Indoctrinator {

			private:

				sll::List<Doctrine> doctrines;

			public:

				Indoctrinator() {}

				void indoctrinate(Doctrine d) { doctrines.append(d); }
				void indoctrinate(Doctrine* d) { doctrines.append(*d); }

				double modify(core::Entity state, core::Entity action) {
					double s = 0;
					for (int i = 0; i < doctrines.getLength(); i++) { s += doctrines.get(i).modify(state, action); }
					return s;
				}

		};

	};

	namespace transactor {

		class Transactor {

			private:

				using TransactionFunctionType = function<core::Entity(core::Entity, core::Entity)>;

				TransactionFunctionType transaction;
				core::Entity currentState;

			public:

				Transactor() {}
				Transactor(TransactionFunctionType t, Entity initialState): transaction(t), currentState(initialState) {}

				core::Entity getState() { return currentState; }

				core::Event transact(Entity action) {
					core::Event evt;
					evt.initialState = currentState;
					evt.takenAction = action;
					currentState = transaction(currentState, action);
					evt.finalState = currentState;
					return evt; // I'm assuming that transaction will always be defined, as the Transactor won't initialize without it
				}

		};

	};

	namespace directive {

		class Directive {

			protected:

				core::Entity state;
				double priority; // high priority = more valuable

			public:

				explicit Directive() {}
				explicit Directive(core::Entity s, double p): state(s), priority(p) {}

				virtual double reward(core::Entity s) = 0;

		};
		
		class MinimizeDistance: public Directive {

			public:

				using Directive::Directive;

				double reward(core::Entity s) override {
					return priority / ((state ^ s) + (1 / ZERO_DISTANCE_REWARD));
				}

		};

		class MaximizeDistance: public Directive {

			public:

				using Directive::Directive;

				double reward(core::Entity s) override {
					return priority * (state ^ s);
				}

		};

		class Director {

			private:

				sll::List<Directive*> directives;

			public:

				Director() {}

				void direct(Directive* d) { directives.append(d); }
				void direct(MinimizeDistance d) { directives.append(&d); }
				void direct(MaximizeDistance d) { directives.append(&d); }

				double reward(core::Entity state) {
					double r = 0;
					for (int i = 0; i < directives.getLength(); i++) { r += directives.get(i).reward(state); }
					return r;
				}

				double reward(core::Event evt) {
					double i = reward(evt.initialState);
					double f = reward(evt.finalState);
					return f - i;
				}

		};

	};

};