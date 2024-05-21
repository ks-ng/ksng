#include <functional>

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

	};

	class Event {

		public:

			Entity initialState;
			Entity takenAction;
			Entity finalState;

			Event(Entity i, Entity a, Entity f): initialState(i), takenAction(a), finalState(f) { }

	};

	class Transactor {



	};

};