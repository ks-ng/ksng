#pragma once
#include "core.h"
#include "../util/dstruct/cutil.h"

namespace v2 {

	inline int getRandomActionNumber(size_t actionCount) {
		return rand() % actionCount;
	}

	class NyxNGV2: public nyxcore::Agent {

		private:

			bool engaged = false;
			sll::SecureLinkedList<nyxcore::Event> memory;
			sda::SecureDataArray<nyxcore::Event> cachedMemory;
			int maximumMemorySize;

			bool isTheseusLimited;
			int theseusLimit;
			int theseus = 0;
			int theseusIterations = 0;
			bool theseusLimitReached = false;

		public:

			NyxNGV2(int maximumMemorySize = 100, bool isTheseusLimited = false, int theseusLimit = 1): maximumMemorySize(maximumMemorySize), 
																									   isTheseusLimited(isTheseusLimited),
																									   theseusLimit(theseusLimit) {}

			string name() override { return "NyxNGV2"; }

			void engage() { engaged = true; cachedMemory = cutil::linkedListToDataArray(memory); }
			void disengage() { engaged = false; }
			void toggle() { engaged = not engaged; }

			void allowFurtherTheseusIterations(int count = 1) { theseusLimit += count; }
			void loadMemory(nyxcore::Event evt) {
				memory.append(evt);
				if (memory.getLength() > maximumMemorySize) {
					memory.remove(0); // remove the earliest memory - maybe this is a bad thing
					if (isTheseusLimited) {
						theseus++;
						if (theseus >= maximumMemorySize) {
							theseus = 0;
							theseusIterations++;
						}
					}
				}
			}
			void loadMemories(sda::SDA<nyxcore::Event> evts) { for (int i = 0; i < evts.getLength(); i++) { loadMemory(evts.get(i)); } }
			void wipeMemory(int index) { memory.remove(index); }
			void wipeMemories(sda::SDA<int> indices) { for (int i = 0; i < indices.getLength(); i++) { wipeMemory(indices.get(i)); } }
			int getMemoryCount() { return memory.getLength(); }

			string dumpMemory() {
				stringstream ss;
				ss << "{" << name() << " memory dump initiated - commence memory dump.";
				for (int i = 0; i < memory.getLength(); i++) {
					ss << "\n\tMemory " << i << ": " << memory.get(i).repr();
				}
				ss << "\nEnd memory dump.\nTotal memory count: " << getMemoryCount() << "}";
				return ss.str();
			}

			void iterate() override {

				if (not engaged) {

					// Play Mode

					theseusLimitReached = (theseusIterations >= theseusLimitReached);
					if (theseusLimitReached) { return; }

					nyxcore::Entity initialState = getState();
					nyxcore::Entity action = getAction(getRandomActionNumber(getActionCount()));
					takeAction(action);
					nyxcore::Entity finalState = getState();
					nyxcore::Event evt({initialState, finalState, action});
					loadMemory(evt);

				} else if (engaged) {

					// Go Mode
					sda::SDA<double> scores(getActionCount());
					for (int i = 0; i < scores.getLength(); i++) { scores.set(i, 0); }
					for (int actionIndex = 0; actionIndex < getActionCount(); actionIndex++) {
						for (int memoryIndex = 0; memoryIndex < getMemoryCount(); memoryIndex++) {
							nyxcore::Entity currentState = getState();
							nyxcore::Entity desiredFinalState = getDirective();
							nyxcore::Entity action = getAction(actionIndex);
							nyxcore::Event mem = cachedMemory.get(memoryIndex);
							double situationalDifference = nyxcore::distance(currentState, mem.getInitialState()) \
								  + nyxcore::distance(desiredFinalState, mem.getFinalState()) \
								  + nyxcore::distance(action, mem.getAction());
							double potentialBenefit = nyxcore::distance(mem.getInitialState(), getDirective()) - nyxcore::distance(mem.getFinalState(), getDirective()); 
							scores.set(actionIndex, scores.get(actionIndex) + (potentialBenefit / situationalDifference));
						}
					}

					int maximumIndex = 0;
					int maximum = -2147483647;
					for (int i = 0; i < scores.getLength(); i++) {
						if (scores.get(i) > maximum) {
							maximumIndex = i;
							maximum = scores.get(i);
						}
					}

					takeAction(getAction(maximumIndex));

				} else {
					notif::fatal("NyxNGV2: impossible state error: engaged & not engaged");
				}

			}

			void takeInSurroundings() {
				nyxcore::Entity initialState = getState();
				disengage();
				for (int i = 0; i < maximumMemorySize; i++) { iterate(); }
				engage();
				brief(initialState);
			}

	};

};