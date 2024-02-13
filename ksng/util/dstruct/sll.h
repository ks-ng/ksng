#include "../notif.h"

namespace sll {

	template <typename T>
	class SecureLinkedListElement {

		private:

			T value;
			SecureLinkedListElement<T>* next;

			bool locked;

		public:

			explicit SecureLinkedListElement(T value): value(value), next(nullptr), locked(false) {}

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list element", ALERT);
				} 
			}

			T getValue() { securityCheck(); return value; }
			void setValue(T value_) { securityCheck(); value = value_; }

			SecureLinkedListElement<T>* getNext() { securityCheck(); return next; }
			void setNext(SecureLinkedListElement<T>* next_) { securityCheck(); next = next_; }

	};

	template <typename T>
	using SLLE = SecureLinkedListElement<T>;

	template <typename T>
	class SecureLinkedList {

		private:

			SLLE<T>* head;

			bool locked;

		public:

			// Constructors and destructors

			explicit SecureLinkedList(): head(nullptr), locked(false) {}

			~SecureLinkedList() {
				SLLE<T>* current = head;
				SLLE<T>* nextNode;

				while (current != nullptr) {
					nextNode = current->getNext();
					delete current;
					current = nextNode;
				}
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list", ALERT);
				} 
			}

			// Writing

			void prepend(T value) {
				securityCheck();

				SLLE<T>* newSLLE = new SLLE<T>(value);

				if (head != nullptr) { newSLLE->setNext(head); }

				head = newSLLE;
			}

			void insert(T value, int index) {
				securityCheck();

				SLLE<T>* newSLLE = new SLLE<T>(value);

				if (index == 0) {
					prepend(value);
					return;
				}

				SLLE<T>* current = head;
				for (int i = 0; i < index - 1 && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr) {
					append(value);
					return;
				}

				newSLLE->setNext(current->getNext());
				current->setNext(newSLLE);
			}

			void append(T value) {
				securityCheck();

				SLLE<T>* newSLLE = new SLLE<T>(value);

				if (head == nullptr) {
					head = newSLLE;
				} else {
					SLLE<T>* current = head;
					while (current->getNext() != nullptr) {
						current = current->getNext();
					}

					current->setNext(newSLLE);
				}
			}

			void remove(int index) {
				securityCheck();

				if (head == nullptr) { return; }

				if (index == 0) {
					head = head->getNext();
					return;
				}

				SLLE<T>* current = head;
				for (int i = 0; i < index - 1 && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr || current->getNext() == nullptr) { return; }

				SLLE<T>* temp = current->getNext();
				current->setNext(temp->getNext());
				delete temp;
			}

			void set(int index, T value) {
				securityCheck();

				SLLE<T>* current = head;
				for (int i = 0; i < index && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr) {
					return T();
				}

				current->setValue(value);
			}

			// Reading

			T get(int index) {
				securityCheck();

				SLLE<T>* current = head;
				for (int i = 0; i < index && current != nullptr; i++) { current = current->getNext(); }

				if (current == nullptr) {
					return T();
				}

				return current->getValue();
			}

			int getLength() {
				securityCheck();
				
				int result = 0;
				SLLE<T>* current = head;
				for (int i = 0; current != nullptr; i++) { current = current->getNext(); result++; }
				return result;
			}

	};

	template <typename T>
	using SLL = SecureLinkedList<T>;

};