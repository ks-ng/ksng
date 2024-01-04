#pragma once
#include "notif.h"
#include <sstream>

using namespace std;

namespace node {

	template <typename T>
	class Node {

		private:

			int maxChildCount;
			int childCount = 0;
			Node<T>** children;

			T value;
			string name;

			bool locked;
			Severity securityLevel;

		public:

			// Constructors

			Node() {}
			Node(string name, T value, int maxChildCount=0, Severity securityLevel=ALERT, bool locked=false) { 
				initialize(name, value, maxChildCount, securityLevel, locked); 
			}

			void initialize(string name_, T value_, int maxChildCount_=0, Severity securityLevel_=ALERT, bool locked_=false) {
				maxChildCount = maxChildCount_;
				children = new Node<T>*[maxChildCount];
				value = value_;
				name = name_;
				locked = locked_;
				securityLevel = securityLevel_;
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure data array", securityLevel);
				} 
			}

			// Access

			string getName() { securityCheck(); return name; }
			int getChildCount() { securityCheck(); return childCount; }
			T& getValue() { securityCheck(); return value; }
			void setValue(T value_) { securityCheck(); value = value_; }

			bool hasChild(string targetName) {
				for (int i = 0; i < getChildCount(); i++) {
					if (children[i]->getName() == targetName) {
						return true;
					}
				} 
				return false;
			}

			Node<T>& getChild(string targetName) {
				for (int i = 0; i < getChildCount(); i++) {
					if (children[i]->getName() == targetName) {
						return *children[i];
					}
				}

				notif::fatal((string)("no such child with name \"") + targetName + (string)("(from ") + getName() + (string)(")"));
				return *children[0];
			}

			void addChild(Node<T> child) {
				securityCheck();
				if (maxChildCount > 0) {
					children[childCount] = &child;
					childCount++;
				} else {
					notif::fatal((string)("child count already maximized (from ") + getName() + (string)(")"));
				}
			}

			// Utility

			string repr() {
				stringstream ss;
				ss << (string)("<") << getName() << (string)(" [");
				if (getChildCount() > 0) {
					for (int i = 0; i < getChildCount(); i++) {
						ss << children[i]->repr();
						if (i < getChildCount() - 1) {
							ss << ", ";
						} else {
							ss << " ";
						}
					}
				}
				ss << (string)("] (") << getChildCount() << (string)("/") << maxChildCount << (string)(")>");
				return ss.str();
			}

	};

	template <typename T>
	bool isomorphic(Node<T> a, Node<T> b) {
		if (a.getChildCount() != b.getChildCount()) {
			return false;
		}

		if (a.getChildCount() == 0) {
			if (a.getName() == b.getName()) {
				return true;
			} else {
				return false;
			}
		}

		string name;
		for (int i = 0; i < a.getChildCount(); i++) {
			name = a.children[i]->getName();
			if (!b.hasChild(name)) {
				return false;
			}
			if (!isomorphic<T>(a.getChild(name), b.getChild(name))) {
				return false;
			}
		}

		return true;

	}

};