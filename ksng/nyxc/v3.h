#include "core.h"

namespace v3 {

	class NyxNGV3: public nyxcore::Agent {

		private:

			bool initialized = false;
			
			nyxcore::Memory memory;
			sda::SecureDataArray<nyxcore::Entity> sentiments;

		public:

			string name() override { return "NyxNGV3"; }

	};

};