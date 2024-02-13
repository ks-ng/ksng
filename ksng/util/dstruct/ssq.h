#include "sll.h"

namespace ssq {

	template <typename T>
	class SecureStack {

		private:

			sll::SLL<T> linkedList;

		public:

			void push(T value) {
				linkedList.prepend(value);
			}

			T pop() {
				T r = linkedList.get(0);
				linkedList.remove(0);
				return r;
			}

			int getLength() { return l.getLength(); }

	};

	template <typename T>
	using SS = SecureStack<T>;

	template <typename T>
	class SecureQueue {

		private:

			sll::SecureLinkedList<T> l;

		public:

			void enqueue(T value) {
				l.append(value);
			}

			void enq(T value) { enqueue(value); }

			T dequeue() {
				T r = l.get(0);
				l.remove(0);
				return r;
			}

			T deq() { return dequeue(); }

			int getLength() { return l.getLength(); }

	};

	template <typename T>
	using SQ = SecureQueue<T>;

};