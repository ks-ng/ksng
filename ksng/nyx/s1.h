#include <string>
#include <cmath>
#include "../util/dstruct/srec.h"
#include "../util/dstruct/ssql.h"

using namespace std;

namespace s1 { 

	class NyxNextGen_Super1 {

		private:

			srec::SecureRecordFormat actionFormat;
			srec::SecureRecordFormat stateFormat;
			srec::SecureRecordFormat memoryFormat;

		public:

			NyxNextGen_Super1(sda::SDA<string> actionLabels, sda::SDA<string> stateLabels) {
				actionFormat.setNames(actionLabels);
				stateFormat.setNames(stateLabels);
				sda::SDA memoryNames(5);
				memoryNames.set(0, "directive");
				memoryNames.set(1, "initial");
				memoryNames.set(2, "action");
				memoryNames.set(3, "final");
				memoryFormat.setNames(memoryNames);
			}

	};

	using NyxNG_Super1 = NyxNextGen_Super1;
	using NyxNGSuper1 = NyxNextGen_Super1;
	using NNG_Super1 = NyxNextGen_Super1;
	using NNGSuper1 = NyxNextGen_Super1;

};
