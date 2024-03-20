#include "../util/dstruct/srec.h"
#include "../util/dstruct/sll.h"
#include <cmath>

namespace nyx_1 {

    using Entity = sda::SecureDataArray<double>;

    double distance(Entity a, Entity b) {
        double result = 0;
        for (int i = 0; i < a.getLength() && i < b.getLength(); i++) {
            result += pow(a.get(i) - b.get(i), 2);
        }
        return sqrt(result);
    }

    sda::SecureDataArray<string> eventRecordFormatNames = {"initial", "final", "action"};
    using Event = srec::SecureRecord<Entity>;
	Event makeEvent(Entity initialState, Entity finalState, Entity action) {
		Event result(eventRecordFormatNames);
		result.set("initial", initialState);
		result.set("final", finalState);
		result.set("action", action);
		return result;
	}

    using Memory = sll::SecureLinkedList<Event>;

    class NyxNextGeneration_Model1 {

        private:

        public:

            

    };

};