#include "sll.h"

namespace sstack {

	template <typename T>
	class SecureStack {

		private:

			sll::SecureLinkedList<T> stk;
			bool locked;

		public:

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure stack", ALERT);
				} 
			}

			// Access

			void push(T value) { securityCheck(); stk.add(sll::SLLE<T>(value, false), stk.getLength() - 1); }
			T pop() { securityCheck() int x = stk.getLength() - 1; T r = stk.getValue(x); stk.remove(x); return r; }

	};

	template <typename T>
	using SS = SecureStack<T>;

};
