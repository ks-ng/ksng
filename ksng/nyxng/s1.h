#include <string>
#include <cmath>
#include "../util/dstruct/sda.h"
#include "../util/dstruct/ssql.h"

using namespace std;

namespace nyxngc_super_1 {

	class Entity: public sda::SecureDataArray<double> { /* nothing */ };

	using E = Entity;

	double compare(Entity a, Entity b) {
		if (a.getLength() != b.getLength()) { return 0; }
		
		double result;
		for (int i = 0; i < a.getLength(); i++) { result += pow(0.5, -pow(a.get(i) - b.get(i), 2)); }
		return result;
	}

	class NyxNGSuper {

		private:

			ssql::SecureLog<Entity> history;

		public:

			NyxNGSuper(int maxHistorySize) { history.setMaximum(maxHistorySize); }

	};

};