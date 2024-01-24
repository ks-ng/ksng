#include "sll.h"

namespace sstack {

	template <typename T>
	class SecureStack {

		private:

			sll::SecureLinkedList<T> stk;
			bool locked;

	};

};
