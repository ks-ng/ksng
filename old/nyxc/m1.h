#include <cmath>
#include "../util/dstruct/sll.h"
#include "../util/dstruct/sda.h"
#include "../util/dstruct/srec.h"

class m1 {

	using Entity = sda::SecureDataArray<double>;

	double distance(Entity a, Entity b) {
		double s = 0;
		for (int i = 0; i < a.getLength() && i < b.getLength(); i++) {
			s += pow(a.get(i) - b.get(i), 2);
		}
		return sqrt(s);
	}

	sda::SDA<string> eventRecordFormatNames = {"initial", "final", "action"};
	using Event = srec::SecureRecord<Entity>;
	Event makeEvent(Entity initialState, Entity finalState, Entity action) {
		Event result(eventRecordFormatNames);
		result.set("initial", initialState);
		result.set("final", finalState);
		result.set("action", action);
		return result;
	}

	using Memory = sll::SecureLinkedList<Event>;

	class NyxNextGen_Model1 {

	};

};
