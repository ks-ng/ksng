#pragma once
#include "../util/dstruct/sda.h"
#include "../util/dstruct/sll.h"
#include <cmath>

namespace core {

	class Entity: public sda::SecureDataArray<double> {

		public:

			// Initializers

			Entity() {}

			Entity(int length_, bool locked_=false, Severity severityLevel_=ALERT) {
				initialize(length_, locked_, severityLevel_);
			}

			Entity(initializer_list<double> initList) {
				length = initList.size();
				elements = new double[length];
				locked = false;
				securityLevel = ALERT;
				int i = 0;
				for (const auto& val : initList) {
					elements[i++] = val;
				}
			}

			string entityRepr() {
				stringstream ss;
				ss << "[Entity: ";
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if (i != getLength() - 1) {
						ss << ", ";
					}
				}
				ss << "]";
				return ss.str();
			}

	};

	double distance(Entity a, Entity b) {
		double s = 0;
		for (int i = 0; i < a.getLength() && i < b.getLength(); i++) {
			s += pow(a.get(i) - b.get(i), 2);
		}
		return sqrt(s);
	}

	class Event {

		private:

			Entity initialState;
			Entity finalState;
			Entity action;

		public:

			Event() {}

			Event(Entity initialState, Entity finalState, Entity action): initialState(initialState), 
																			finalState(finalState), 
																			action(action) {}

			Event(initializer_list<Entity> initList) {
				int i = 0;
				for (const auto& val : initList) {
					if (i == 0) {
						initialState = val;
					} else if (i == 1) {
						finalState = val;
					} else if (i == 2) {
						action = val;
					} else {
						break;
					}
					i++;
				}
				if (i < 2) {
					notif::fatal("NyxNG: not enough Entities in initializer list to create Event");
				}
			}

			inline Entity getInitialState() { return initialState; }
			inline Entity getFinalState() { return finalState; }
			inline Entity getAction() { return action; }

			inline void setInitialState(Entity is) { initialState = is; }
			inline void setFinalState(Entity fs) { finalState = fs; }
			inline void setAction(Entity a) { action = a; }

			string repr() {
				stringstream ss;
				ss << "[Event: " << getInitialState().entityRepr() << " -> " << getFinalState().entityRepr() << " by " << getAction().entityRepr() << "]";
				return ss.str();
			}

	};

	class Memory: public sll::SecureLinkedList<Event> {

		public:

			Memory() {
				head = nullptr;
				locked = false;
			}

			Memory(initializer_list<Event> initList) {
				head = nullptr;
				locked = false;
				int i = 0;
				for (const auto& val : initList) {
					append(val);
				}
			}

			// ~Memory() {
			// 	sll::SecureLinkedListElement<Event>* current = head;
			// 	sll::SecureLinkedListElement<Event>* nextNode;

			// 	while (current != nullptr) {
			// 		nextNode = current->getNext();
			// 		delete current;
			// 		current = nextNode;
			// 	}
			// }

			string repr() {
				stringstream ss;
				ss << "[Memory:\n";
				for (int i = 0; i < getLength(); i++) {
					ss << "\tEvent " << i << ": " << get(i).repr() << "\n";
				}
				ss << "End memory.]";
				return ss.str();
			}

	};

	class Actuator {

		public:

			virtual void delta(Entity state, Entity action) = 0;

	};

	class Agent {

		private:

			Entity state;
			Entity directive;
			sda::SDA<Entity> actions;
			Actuator* actuator;

			bool installed;
			bool instructed;
			bool briefed;
			bool directed;

		public:

			void install(Actuator* a) { actuator = a; installed = true; }
			Entity delta(Entity state, Entity action) { return actuator->delta(state, action); }
			void instruct(sda::SDA<Entity> as) { actions = as; instructed = true; }
			void brief(Entity s) { state = s; briefed = true; }
			void direct(Entity d) { directive = d; directed = true; }

			inline Entity getState() { return state; }
			inline Entity getDirective { return directive; }
			inline sda::SDA<Entity> getActions() { return actions; }
			inline Entity getAction(int index) { return actions.get(index); }
			inline int getActionCount() { return actions.getLength(); }
			inline Actuator* getActuator() { return actuator; }

			bool actuatorInstalled() { return installed; }
			bool wasInstructed() { return instructed; }
			bool wasBriefed() { return briefed; }
			bool wasDirected() { return directed; }
			bool isReady() { return actuatorInstalled() && wasInstructed() && wasBriefed() && wasDirected(); }

			void takeAction(Entity action) { state = delta(state, action); }

			virtual void iterate() = 0;
			Entity run(int x) { if (!isReady()) {notif::warning("agent not ready"); return state; } for (int i = 0; i < x; i++) { iterate(); } return state; }

	};

};