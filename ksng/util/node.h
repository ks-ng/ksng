#include "notif.h"
#include <sstream>

using namespace std;

namespace node {

	template <typename T>
	class Node {

		private:

			string name;
			T value;
			Node<T>* children;
			int childCount = 0;
			int maxChildCount;

		public:

			// Constructors

			Node() {}
			Node(string name_, T value_, int maxChildCount_=0) { 
				initialize(name_, value_, maxChildCount_);
			}

			void initialize(string name_, T value_, int maxChildCount_=0) {
				name = name_;
				name = name_;
				value = value_;
				maxChildCount = maxChildCount_;
				children = new Node<T>[maxChildCount];
			}

			// Access

			// - reading

			// - - reading properties

			string getName() { return name; }
			T getValue() { return value; }

			// - - reading children
			int getChildCount() { return childCount; }
			int getMaxChildCount() { return maxChildCount; }
			bool hasChild(string targetName) {
				for (int i = 0; i < childCount; i++) { if (children[i].getName() == targetName) { return true; } }
				return false;
			}
			Node<T>& getChild(string targetName) {
				for (int i = 0; i < childCount; i++) { 
					if (children[i].getName() == targetName) { 
						return children[i]; 
					} 
				}
				stringstream ss;
				ss << "\"" << getName() << "\" does not have child \"" << targetName << "\"";
				notif::fatal(ss.str());
				return *this; // don't worry, it won't ever get called unless something AWFUL is happening
			}
			Node<T>& getChild(int index) {
				if (index < childCount) { return children[index]; } else { notif::fatal("index out of range"); return *this; }
			}

			// - writing

			void addChild(Node<T>& child) {
				if (childCount >= maxChildCount) {
					stringstream ss;
					ss << "\"" << getName() << "\" is full";
					notif::fatal(ss.str());
				} else {
					children[childCount] = child;
					childCount++;
				}
			}

			// Utility

			string repr() {
				stringstream ss;
				ss << "<" << getName() << " [";
				if (getChildCount() > 0) {
					for (int i = 0; i < getChildCount(); i++) {
						ss << getChild(i).repr();
						if (i < getChildCount() - 1) {
							ss << ", ";
						}
					}
				}
				ss << "] (" << getChildCount() << "/" << getMaxChildCount() << ")>";
				return ss.str();
			}

	};

};