#pragma once
#include "../notif.h"

namespace sll {

	template <typename T>
	class SecureLinkedListElement {

		private:

			T value;
			SecureLinkedListElement<T>* last = nullptr;
			SecureLinkedListElement<T>* next = nullptr;
			bool locked;

		public:

			SecureLinkedListElement(T value, bool locked): value(value), locked(locked) {}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list element", ALERT);
				} 
			}

			// Access

			T& getValue() { securityCheck(); return value; }
			void setValue(T v) { securityCheck(); value = v; }
			SecureLinkedListElement<T> getNext() { 
				securityCheck(); 
				if (next == nullptr) { notif::fatal("reached past the end of linked list (segfault prevented)"); }
				next->securityCheck(); 
				return *next; 
			}
			void setNext(SecureLinkedListElement<T> other) { 
				securityCheck(); 
				if (other != nullptr) {
					other.securityCheck(); 
				}
				next = other; 
			}
			SecureLinkedListElement<T> getLast() { 
				securityCheck(); 
				if (last == nullptr) { notif::fatal("reaches past the beginning of linked list (segfault prevented)"); }
				last->securityCheck(); 
				return *last; 
			}
			void setLast(SecureLinkedListElement<T> other) { 
				securityCheck(); 
				if (other != nullptr) {
					other.securityCheck(); 
				}
				last = &other; 
			}
			bool hasNext() { return next != nullptr; }
			bool hasLast() { return last != nullptr; }

	};

	template <typename T>
	using SLLE = SecureLinkedListElement<T>;

	template <typename T>
	class SecureLinkedList {

		private:

			int cachedLength;
			bool regenerateLength;

		public:

			SLLE<T>* first = nullptr;

			explicit SecureLinkedList() {}
			explicit SecureLinkedList(SLLE<T> first): first(first) {}

			// Security

			void reveal() { for (int i = 0; i < getLength(); i++) { get(i).reveal(); } }
			void hide() { for (int i = 0; i < getLength(); i++) { get(i).hide(); } }

			// Access

			void setFirst(SSLE<T> elem) { first = &elem; regenerateLength = true; }

			int getLength() {
				if (regenerateLength) {
					if (first == nullptr) { return 0; }
					int l = 0;
					SLLE<T>* ptr = first;
					while (ptr->hasNext()) {
						l++;
						ptr = &(ptr->getNext());
					}
					cachedLength = l;
					regenerateLength = false;
					return l;
				} else {
					return cachedLength;
				}
			}

			SLLE<T>& get(int i) {
				SLLE<T>* ptr = first;
				for (int j = 0; j < i; j++) { ptr = &(ptr->getNext()); }
				return *ptr;
			}

			T& getValue(int i) {
				return get(i).getValue();
			}

			void add(SLLE<T> element, int i) {
				regenerateLength = true;
				if (getLength() == 0) { setFirst(element); return; }
				if (i == 0) {
					element.setNext(first);
					first->setLast(&element);
					first = &element;
				} else if (i == getLength() - 1) {
					SLLE<T>* lst = &(get(getLength() - 1));
					element.setLast(lst);
					lst->setNext(&element);
				} else if (i >= 0 && i < getLength()) {
					SLLE<T>* lst = &get(i);
					SLLE<T>* nxt = &get(i + 1);
					lst->setNext(&element);
					nxt->setLast(&element);
					element.setLast(lst);
					element.setNext(nxt);
				} else {
					notif::fatal("invalid access index on secure linked list");
				}
			}

			void remove(int i) {
				regenerateLength = true;
				if (getLength() == 1) { first = nullptr; return; }
				if (i == 0) {
					get(1).setLast(nullptr);
					first = &get(1);
				} else if (i == getLength() - 1) {
					get(getLength() - 1).setLast(nullptr);
				} else if (i >= 0 && i < getLength()) {
					SLLE<T>* lst = &get(i - 1);
					SLLE<T>* nxt = &get(i + 1);
					lst->setNext(nxt);
					nxt->setLast(lst);
				}
			}

	};

};
