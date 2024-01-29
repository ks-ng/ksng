#include "sll.h"

namespace ssq {

	template <typename T>
	class SecureStack: public sll::SecureLinkedList<T> {

		public:

			void push(T value) {
				prepend(value);
			}

			T pop() {
				T r = get(0);
				remove(0);
				return r;
			}

	};

	template <typename T>
	using SS = SecureStack<T>;

	template <typename T>
	class SecureQueue: public sll::SecureLinkedList<T> {

		public:

			void enqueue(T value) {
				append(value);
			}

			void enq(T value) { enqueue(value); }

			T dequeue() {
				T r = get(0);
				remove(0);
				return r;
			}

			T deq() { return dequeue(); }

	};

	template <typename T>
	using SQ = SecureQueue<T>

};