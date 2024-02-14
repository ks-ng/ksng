#pragma once

#include "sll.h"
#include <sstream>

namespace snode {

	template <typename T>
	class SecureNode {
		
		private:

			bool locked = false;
			string label;
			T value;
			sll::SecureLinkedList<SecureNode<T>*> children;

			int getIndex(string name) {
				for (int i = 0; i < children.getLength(); i++) {
					if (children.get(i)->getName() == name) {
						return i;
					}
				}
				return -1;
			}

		public:

			SecureNode() {}
			SecureNode(string label, T value): label(label), value(value) {}

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure linked list", ALERT);
				} 
			}

			T getValue() { securityCheck(); return value; }
			string getName() { securityCheck(); return label; }
			SecureNode<T> getChild(string name) { return *(children.get(getIndex(name))); }
			SecureNode<T> getChildByIndex(int i) { return *(children.get(i)); }

			void attach(SecureNode<T> child) {
				children.append(&child);
			}

			void detach(string name) {
				int index = getIndex(name);
				if (index != -1) {
					children.remove(index);
				}
			}

			bool has(string name) { return getIndex(name) != -1; }
			string getNameByIndex(int index) { return children.get(index)->getName(); }

			string repr() {
				stringstream ss;
				ss << "<" << label << ": " << value;
				if (children.getLength() > 0) {
					ss << " with children ";
					for (int i = 0; i < children.getLength(); i++) {
						ss << getChildByIndex(i).repr();
						if (i != children.getLength() - 1) {
							ss << ", ";
						}
					}
				}
				ss << ">";
				return ss.str();
			}

	};

	template <typename T>
	using SN = SecureNode<T>;

};