#include "sll.h"

namespace ssql {

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

	};

	template <typename T>
	using SS = SecureStack<T>;

	template <typename T>
	class SecureQueue: public sll::SecureLinkedList<T> {

		private:

			sll::SLL<T> linkedList;

		public:

			void enqueue(T value) {
				linkedList.append(value);
			}

			void enq(T value) { linkedList.enqueue(value); }

			T dequeue() {
				T r = linkedList.get(0);
				linkedList.remove(0);
				return r;
			}

			T deq() { return dequeue(); }

	};

	template <typename T>
	using SQ = SecureQueue<T>;

	template <typename T>
	class SecureLog: public sll::SecureLinkedList<T> {

		private:

			int maximum;
			int currentSize = 0;
			sll::SLL<T> linkedList;

		public:

			SecureLog(int maximum): maximum(maximum) {}

			void enqueue(T value) {
				linkedList.append(value);
				if (currentSize < maximum) { currentSize++; } else { dequeue(); }
			}

			void enq(T value) { enqueue(value); }

			T dequeue() {
				T r = linkedList.get(0);
				linkedList.remove(0);
				return r;
			}

			T deq() { return dequeue(); }

			int getLength() { return currentSize; }
			T get(int i) { return linkedList.get(i); }

	};

};