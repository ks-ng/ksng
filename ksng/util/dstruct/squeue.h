#include "sll.h"

namespace squeue {

	template <typename T>
	class SecureQueue {

		private:

			sll:SecureLinkedList<T> q;
			bool locked;

		public:

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure queue", ALERT);
				} 
			}

			// Access

			void enqueue(T value) { q.add(0, sll::SLLE<T>(value, false)); }
			T dequeue() { int x = q.getLength() - 1; T r = q.getValue(x); q.remove(x); return r; }

	};

};
