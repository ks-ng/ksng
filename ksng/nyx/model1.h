#include <string>
#include <cmath>
#include "../util/dstruct/srec.h"
#include "../util/dstruct/ssq.h"

using namespace std;

namespace s1 { 

	using Entity = srec::SecureRecord<int>;
	using Memory = srec::SecureRecord<Entity>;

	enum Engagement { engaged, disengaged };

	class NyxNextGen_Model1 {

		private:

			srec::SecureRecordFormat<int> actionFormat;
			srec::SecureRecordFormat<int> stateFormat;
			srec::SecureRecordFormat<Entity> memoryFormat;

			Entity directive;
			Entity state;

			sll::SecureLinkedList<Entity> actions;
			
			ssq::SecureQueue<Memory> memories; // queue of memories (records of records of int)
			int memorySize;

			Engagement engagement = disengaged;

		public:

			NyxNextGen_Model1(sda::SDA<string> actionLabels, sda::SDA<string> stateLabels, int memorySize = 100): memorySize(memorySize) {
				actionFormat.setNames(actionLabels);
				stateFormat.setNames(stateLabels);
				sda::SDA<string> memoryNames(4);
				memoryNames.set(1, "initial");
				memoryNames.set(2, "action");
				memoryNames.set(3, "final");
				memoryFormat.setNames(memoryNames);
			}

			// Functionality

			// // Memory handling

			void remember(Memory memory) {
				memories.enqueue(memory);
				if (memories.getLength() > memorySize) {
					memories.dequeue();
				}
			}

			// // Action handling

			virtual Entity calculateNewState(Entity action) = 0;

			void takeAction(Entity action) {
				// Take the specified action
				state = calculateNewState(action);
			}

			double scoreEntity(Entity a, Entity b) {
				double result = 0;
				string name;
				for (int i = 0; i < state.getLength(); i++) {
					name = a.getName(i);
					result += pow(0.5, pow(a.get(name) - b.get(name), 2));
				}
				return result / a.getLength();
			}

			double scoreAction(Entity initialState, Entity action, Entity targetState) {
				double result;
				Memory memory;
				Memory otherMemory;
				for (int i = 0; i < memories.getLength(); i++) {
					memory = memories.get(i);
					result += scoreEntity(memory.get((string)("initial")), initial)
					        * scoreEntity(memory.get((string)("action")), action)
							* scoreEntity(memory.get((string)("final")), targetState);
					for (int j = 0; j < memories.getLength(); j++) {
						if (i == j) { continue; }
						result += 
					}
				}
			}

			// // User actions

			void addNewAction(Entity action) { 
				// Add a new action to the AI's capabilities
				actions.append(action); 
			}

			void indoctrinate(Memory memory) { 
				// Force a new memory into the AI
				remember(memory); 
			}

			void direct(Entity targetState) {
				// Direct the AI to a new state
				directive = targetState;
			}

			// NyxNG actions

			void iterate() {
				// please work on this when you are at your worst, i know you can do it - past lux
			}

	};

	using NyxNG_Model1 = NyxNextGen_Model1;
	using NyxNGModel1 = NyxNextGen_Model1;
	using NNG_Model1 = NyxNextGen_Model1;
	using NNGModel1 = NyxNextGen_Model1;
	using Model1 = NyxNextGen_Model1;

};