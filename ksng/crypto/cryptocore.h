#pragma once

#include "../util/fileops.h"

namespace cryptocore {

	bool HIGH_SECURITY = false;

	class Key {

		// Key class adds custom behavior to the Data class and allows
		// securely loading and saving keys to files, as well as generating
		// them securely

		private:

			Data data;

		public:

			// Constructors & destructors

			Key(int length): data(datacls::Data(length)) {
				data.hide();
			}

			~Key() {
				data.erase();
			}

			// Security is built into the datacls::Data so it's not necessary here	

	};

};