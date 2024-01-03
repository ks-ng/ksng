#pragma once
#include "notif.h"

using namespace std;

namespace node {

	template <typename T>
	class Node {

		private:

			int maxChildCount;
			int childCount = 0;
			Node<T>* children;

			T value;
			string name;

			bool locked;
			Severity securityLevel;

		public:

			// Constructors

			Node() {}
			Node(string name, T value, int maxChildCount=0, Severity securityLevel=ALERT, bool locked=false) { 
				initialize(name, value, maxChildCount, securityLevel, bool locked); 
			}

			void initialize(string name_, T value_, int maxChildCount_=0, Severity securityLevel_=ALERT, bool locked_=false) {
				maxChildCount = maxChildCount_;
				children = new Node<T>[maxChildCount];
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

			Node<T>& getChild(string name) {
				for (int i = 0; i < childCount; i++) {

				}

				notif::error((string)("no such child with name \"") + name);
				return *this;
			}

	};

};