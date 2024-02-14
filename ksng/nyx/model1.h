#include <string>
#include <cmath>
#include "../util/dstruct/srec.h"
#include "../util/dstruct/ssq.h"

using namespace std;

namespace s1 { 

	using Entity = srec::SecureRecord<int>;
	using Memory = srec::SecureRecord<Entity>

	enum Engagement { engaged, disengaged }

	double computeSimilarity()

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

			NyxNextGen_Super1(sda::SDA<string> actionLabels, sda::SDA<string> stateLabels, int memorySize = 100): memorySize(memorySize) {
				actionFormat.setNames(actionLabels);
				stateFormat.setNames(stateLabels);
				sda::SDA<string> memoryNames(4);
				memoryNames.set(0, "directive");
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
