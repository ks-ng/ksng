#include <string>
#include <cmath>
#include "../util/dstruct/srec.h"
#include "../util/dstruct/ssql.h"

using namespace std;

namespace nyx_super1 {

	class NyxNextGen_Super {

		private:

			srec::SecureRecordFormat actionFormat;
			srec::SecureRecordFormat stateFormat;
			srec::SecureRecordFormat memoryFormat;

		public:

			NyxNextGen_Super(sda::SDA<string> actionLabels, sda::SDA<string> stateLabels) {
				actionFormat.setNames(actionLabels);
				stateFormat.setNames(stateLabels);
			}

	};

	using NyxNG_Super = NyxNextGen_Super;
	using NyxNGSuper = NyxNextGen_Super;
	using NNG_Super = NyxNextGen_Super;
	using NNGSuper = NyxNextGen_Super;

};